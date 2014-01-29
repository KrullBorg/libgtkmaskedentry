/*
 * GtkCellRendererMasked widget for GTK+
 * Copyright (C) 2005-2014 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <gtkmaskedentry.h>

#include "gtkcellrenderermasked.h"

static void gtk_cell_renderer_masked_init (GtkCellRendererMasked *cell);
static void gtk_cell_renderer_masked_class_init (GtkCellRendererMaskedClass *class);


static void gtk_cell_renderer_masked_get_property (GObject *object,
                                                   guint param_id,
                                                   GValue *value,
                                                   GParamSpec *pspec);
static void gtk_cell_renderer_masked_set_property (GObject *object,
                                                   guint param_id,
                                                   const GValue *value,
                                                   GParamSpec *pspec);

static GtkCellEditable *gtk_cell_renderer_masked_start_editing (GtkCellRenderer *cell,
                                                                GdkEvent *event,
                                                                GtkWidget *widget,
                                                                const gchar *path,
                                                                const GdkRectangle *background_area,
                                                                const GdkRectangle *cell_area,
                                                                GtkCellRendererState flags);


enum
{
	PROP_0,
	PROP_MASK
};

static gpointer parent_class;

#define GTK_CELL_RENDERER_TEXT_PATH "gtk-cell-renderer-text-path"

#define GTK_CELL_RENDERER_MASKED_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_CELL_RENDERER_MASKED, GtkCellRendererMaskedPrivate))

typedef struct _GtkCellRendererMaskedPrivate GtkCellRendererMaskedPrivate;
struct _GtkCellRendererMaskedPrivate
{
	gchar *mask;

	GtkWidget *entry;
};

G_DEFINE_TYPE (GtkCellRendererMasked, gtk_cell_renderer_masked, GTK_TYPE_CELL_RENDERER_TEXT)

static void
gtk_cell_renderer_masked_init (GtkCellRendererMasked *cell)
{
  GtkCellRendererMaskedPrivate *priv = GTK_CELL_RENDERER_MASKED_GET_PRIVATE (cell);

  priv->mask = g_strdup ("");
}

static void
gtk_cell_renderer_masked_class_init (GtkCellRendererMaskedClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (class);
	GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (class);

	parent_class = g_type_class_peek_parent (class);

	object_class->get_property = gtk_cell_renderer_masked_get_property;
	object_class->set_property = gtk_cell_renderer_masked_set_property;

	cell_class->start_editing = gtk_cell_renderer_masked_start_editing;

	g_object_class_install_property (object_class,
                                   PROP_MASK,
                                   g_param_spec_string ("mask",
                                                        "Mask",
                                                        "Mask",
                                                        NULL,
                                                        G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GtkCellRendererMaskedPrivate));
}

static void
gtk_cell_renderer_masked_get_property (GObject *object,
                                       guint param_id,
                                       GValue *value,
                                       GParamSpec *pspec)
{
	GtkCellRendererMasked *celltext = GTK_CELL_RENDERER_MASKED (object);
	GtkCellRendererMaskedPrivate *priv;

	priv = GTK_CELL_RENDERER_MASKED_GET_PRIVATE (object);

	switch (param_id)
		{
			case PROP_MASK:
				g_value_set_string (value, priv->mask);
				break;
		}
}

static void
gtk_cell_renderer_masked_set_property (GObject *object,
                                       guint param_id,
                                       const GValue *value,
                                       GParamSpec *pspec)
{
	GtkCellRendererMasked *celltext = GTK_CELL_RENDERER_MASKED (object);

	switch (param_id)
		{
			case PROP_MASK:
				{
					gtk_cell_renderer_masked_set_mask (celltext, g_value_get_string (value));
					break;
				}
		}
}

/**
 * gtk_cell_renderer_masked_new:
 *
 * Creates a new #GtkCellRendererMasked widget.
 *
 * Returns: the newly created #GtkCellRendererMasked
 */
GtkCellRenderer
*gtk_cell_renderer_masked_new ()
{
	return g_object_new (GTK_TYPE_CELL_RENDERER_MASKED, NULL);
}

/**
 * gtk_cell_renderer_masked_new_with_mask:
 * @mask: the mask.
 *
 * Creates a new #GtkCellRendererMasked widget.
 *
 * Returns: the newly created #GtkCellRendererMasked
 */
GtkCellRenderer
*gtk_cell_renderer_masked_new_with_mask (const gchar *mask)
{
	GtkCellRenderer *r = gtk_cell_renderer_masked_new();

	gtk_cell_renderer_masked_set_mask (GTK_CELL_RENDERER_MASKED (r), mask);

	return r;
}

static void
gtk_cell_renderer_masked_editing_done (GtkCellEditable *entry,
                                       gpointer data)
{
	const gchar *path, *new_text;

	gboolean editing_canceled;

	GtkCellRendererMaskedPrivate *priv = GTK_CELL_RENDERER_MASKED_GET_PRIVATE (data);

	priv->entry = NULL;

	g_object_get (G_OBJECT (entry), "editing-canceled", &editing_canceled, NULL);
	gtk_cell_renderer_stop_editing (GTK_CELL_RENDERER (data),
	                                editing_canceled);
	if (editing_canceled)
		return;

	path = g_object_get_data (G_OBJECT (entry), GTK_CELL_RENDERER_TEXT_PATH);
	new_text = gtk_entry_get_text (GTK_ENTRY (entry));

	g_signal_emit_by_name (data, "edited", path, new_text);
}

static GtkCellEditable
*gtk_cell_renderer_masked_start_editing (GtkCellRenderer *cell,
                                         GdkEvent *event,
                                         GtkWidget *widget,
                                         const gchar *path,
                                         const GdkRectangle *background_area,
                                         const GdkRectangle *cell_area,
                                         GtkCellRendererState flags)
{
	GtkCellRendererMasked *celltext;
	GtkCellRendererMaskedPrivate *priv;

	gboolean editable;
	gfloat xalign;
	gchar *text;

	celltext = GTK_CELL_RENDERER_MASKED (cell);
	priv = GTK_CELL_RENDERER_MASKED_GET_PRIVATE (cell);

	/* If the cell isn't editable we return NULL. */
	g_object_get (G_OBJECT (celltext), "editable", &editable, NULL);
	if (editable == FALSE)
		return NULL;

	g_object_get (G_OBJECT (cell), "xalign", &xalign, NULL);
	priv->entry = g_object_new (GTK_TYPE_MASKED_ENTRY,
	                            "has-frame", FALSE,
	                            "xalign", xalign,
	                            NULL);

	if (priv->mask)
		gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (priv->entry), priv->mask);

	text = NULL;
	g_object_get (G_OBJECT (celltext), "text", &text, NULL);
	if (text)
		gtk_entry_set_text (GTK_ENTRY (priv->entry), text);

	g_object_set_data_full (G_OBJECT (priv->entry), GTK_CELL_RENDERER_TEXT_PATH, g_strdup (path), g_free);

	gtk_editable_select_region (GTK_EDITABLE (priv->entry), 0, -1);

	g_signal_connect (priv->entry,
	                  "editing_done",
	                  G_CALLBACK (gtk_cell_renderer_masked_editing_done),
	                  celltext);

	gtk_widget_show (priv->entry);

	return GTK_CELL_EDITABLE (priv->entry);
}

/**
 * gtk_cell_renderer_masked_set_mask:
 * @renderer: a #GtkCellRendererMasked widget.
 * @mask: the mask.
 *
 * Set the mask. View gtk_masked_entry_new_with_mask() for possible values.
 */
void
gtk_cell_renderer_masked_set_mask (GtkCellRendererMasked *renderer,
                                   const gchar *mask)
{
	GtkCellRendererMaskedPrivate *priv = GTK_CELL_RENDERER_MASKED_GET_PRIVATE (renderer);

	g_object_set (GTK_CELL_RENDERER_TEXT (renderer), "text", "", NULL);
	priv->mask = g_strdup (mask);
}
