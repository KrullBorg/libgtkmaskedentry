/*
 * Copyright (C) 2010 Andrea Zagli <azagli@libero.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gtk/gtk.h>

#include "gtkmaskedentry.h"
#include "gtkformwidgetmaskedentry.h"

enum
{
	PROP_0,
	PROP_STRINGIFY_WITH_MASK
};

static void gtk_form_widget_masked_entry_class_init (GtkFormWidgetMaskedEntryClass *klass);
static void gtk_form_widget_masked_entry_init (GtkFormWidgetMaskedEntry *gtk_form_widget_masked_entry);

static void gtk_form_widget_masked_entry_set_property (GObject *object,
                                      guint property_id,
                                      const GValue *value,
                                      GParamSpec *pspec);
static void gtk_form_widget_masked_entry_get_property (GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

#define GTK_FORM_WIDGET_MASKED_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_FORM_TYPE_WIDGET_MASKED_ENTRY, GtkFormWidgetMaskedEntryPrivate))

typedef struct _GtkFormWidgetMaskedEntryPrivate GtkFormWidgetMaskedEntryPrivate;
struct _GtkFormWidgetMaskedEntryPrivate
	{
		gboolean stringify_with_mask;
	};


G_DEFINE_TYPE (GtkFormWidgetMaskedEntry, gtk_form_widget_masked_entry, TYPE_GTK_FORM_WIDGET)

static void
gtk_form_widget_masked_entry_class_init (GtkFormWidgetMaskedEntryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkFormWidgetClass *widget_class = GTK_FORM_WIDGET_CLASS (klass);

	object_class->set_property = gtk_form_widget_masked_entry_set_property;
	object_class->get_property = gtk_form_widget_masked_entry_get_property;

	widget_class->get_value_stringify = gtk_form_widget_masked_entry_get_value_stringify;
	widget_class->set_value_stringify = gtk_form_widget_masked_entry_set_value_stringify;
	widget_class->set_editable = gtk_form_widget_masked_entry_set_editable;

	g_object_class_install_property (object_class, PROP_STRINGIFY_WITH_MASK,
	                                 g_param_spec_boolean ("stringify-with-mask",
	                                                   "GtkFormWidgetMaskedEntry::get_value_stringify returns with mask",
	                                                   "GtkFormWidgetMaskedEntry::get_value_stringify returns with mask",
	                                                   TRUE,
	                                                   G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GtkFormWidgetMaskedEntryPrivate));
}

static void
gtk_form_widget_masked_entry_init (GtkFormWidgetMaskedEntry *gtk_form_widget_masked_entry)
{
}

/**
 * gtk_form_widget_masked_entry_new:
 *
 * Returns: the newly created #GtkFormWidgetMaskedEntry.
 */
GtkFormWidget
*gtk_form_widget_masked_entry_new ()
{
	return g_object_new (gtk_form_widget_masked_entry_get_type (), NULL);
}

/**
 * gtk_form_widget_masked_entry_get_value_stringify:
 * @widget:
 *
 */
gchar
*gtk_form_widget_masked_entry_get_value_stringify (GtkFormWidget *fwidget)
{
	GtkFormWidgetMaskedEntryPrivate *priv = GTK_FORM_WIDGET_MASKED_ENTRY_GET_PRIVATE (fwidget);

	GtkWidget *w = gtk_form_widget_get_widget (fwidget);

	if (priv->stringify_with_mask)
		{
			return g_strdup (gtk_entry_get_text (GTK_ENTRY (w)));
		}
	else
		{
			return g_strdup (gtk_masked_entry_get_text (GTK_MASKED_ENTRY (w)));
		}
}

/**
 * gtk_form_widget_masked_entry_set_value_stringify:
 * @fwidget:
 * @value:
 *
 */
gboolean
gtk_form_widget_masked_entry_set_value_stringify (GtkFormWidget *fwidget, const gchar *value)
{
	gboolean ret = FALSE;
	GtkWidget *w;

	g_object_get (G_OBJECT (fwidget),
	              "widget", &w,
	              NULL);

	gtk_entry_set_text (GTK_ENTRY (w), value);

	ret = TRUE;

	return ret;
}

/**
 * gtk_form_widget_set_editable:
 * @fwidget:
 * @editable:
 *
 */
void
gtk_form_widget_masked_entry_set_editable (GtkFormWidget *fwidget, gboolean editable)
{
	GtkWidget *w;

	g_object_get (G_OBJECT (fwidget),
	              "widget", &w,
	              NULL);

	gtk_editable_set_editable (GTK_EDITABLE (w), editable);
}

/* PRIVATE */
static void
gtk_form_widget_masked_entry_set_property (GObject *object,
                                guint property_id,
                                const GValue *value,
                                GParamSpec *pspec)
{
	GtkFormWidgetMaskedEntry *widget_entry = (GtkFormWidgetMaskedEntry *)object;

	GtkFormWidgetMaskedEntryPrivate *priv = GTK_FORM_WIDGET_MASKED_ENTRY_GET_PRIVATE (widget_entry);

	switch (property_id)
		{
			case PROP_STRINGIFY_WITH_MASK:
				priv->stringify_with_mask = g_value_get_boolean (value);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}

static void
gtk_form_widget_masked_entry_get_property (GObject *object,
                                guint property_id,
                                GValue *value,
                                GParamSpec *pspec)
{
	GtkFormWidgetMaskedEntry *widget_entry = (GtkFormWidgetMaskedEntry *)object;

	GtkFormWidgetMaskedEntryPrivate *priv = GTK_FORM_WIDGET_MASKED_ENTRY_GET_PRIVATE (widget_entry);

	switch (property_id)
		{
			case PROP_STRINGIFY_WITH_MASK:
				g_value_set_boolean (value, priv->stringify_with_mask);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
	  }
}
