/*
 * GtkMaskedEntry widget for GTK+
 * Copyright (C) 2005-2011 Andrea Zagli <azagli@libero.it>
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

#include <ctype.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>

#include "gtkmaskedentry.h"

typedef enum
{
	GTK_MASKED_ENTRY_LEFT   = -1,
	GTK_MASKED_ENTRY_RIGHT  = 1
} GtkMaskedEntryDirections;

enum
{
	PROP_0,
	PROP_MASK,
	PROP_TAB_INSIDE
};

static void gtk_masked_entry_class_init (GtkMaskedEntryClass *klass);
static void gtk_masked_entry_init (GtkMaskedEntry *masked_entry);

static gboolean gtk_masked_entry_key_press_event (GtkWidget   *widget,
                                                  GdkEventKey *event,
                                                  gpointer     user_data);
static void gtk_masked_entry_delete_text      (GtkEditable *editable,
                                               gint         start_pos,
                                               gint         end_pos,
                                               gpointer     user_data);
static void gtk_masked_entry_insert_text      (GtkEditable *editable,
                                               const gchar *text,
                                               gint         length,
                                               gint        *position,
                                               gpointer     user_data);

static gboolean gtk_masked_entry_is_writeable (gchar c);
static gint gtk_masked_entry_get_first_writeable_mask (GtkMaskedEntry *masked_entry,
                                                       GtkMaskedEntryDirections direction);
static gint gtk_masked_entry_get_next_writeable_block (GtkMaskedEntry *masked_entry,
                                                       GtkMaskedEntryDirections direction);
static void gtk_masked_entry_get_block_positions (GtkMaskedEntry *masked_entry,
                                                  gint *start,
                                                  gint *end);

static void gtk_masked_entry_set_property (GObject *object,
                                           guint property_id,
                                           const GValue *value,
                                           GParamSpec *pspec);
static void gtk_masked_entry_get_property (GObject *object,
                                           guint property_id,
                                           GValue *value,
                                           GParamSpec *pspec);

#define GTK_MASKED_ENTRY_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GTK_TYPE_MASKED_ENTRY, GtkMaskedEntryPrivate))

typedef struct _GtkMaskedEntryPrivate GtkMaskedEntryPrivate;
struct _GtkMaskedEntryPrivate
	{
		gchar *mask;
		gboolean tab_inside;
	};

G_DEFINE_TYPE (GtkMaskedEntry, gtk_masked_entry, GTK_TYPE_ENTRY)

static void
gtk_masked_entry_class_init (GtkMaskedEntryClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (object_class, sizeof (GtkMaskedEntryPrivate));

	object_class->set_property = gtk_masked_entry_set_property;
	object_class->get_property = gtk_masked_entry_get_property;

	g_object_class_install_property (object_class, PROP_MASK,
	                                 g_param_spec_string ("mask",
	                                                      "The mask",
	                                                      "The mask",
	                                                      "",
	                                                      G_PARAM_READWRITE));
	g_object_class_install_property (object_class, PROP_TAB_INSIDE,
	                                 g_param_spec_boolean ("tab-inside",
	                                                       "Tabulation inside the widget",
	                                                       "Whether pressing tab moves between mask's parts or outside the widget",
	                                                       FALSE,
	                                                       G_PARAM_READWRITE));
}

static void
gtk_masked_entry_init (GtkMaskedEntry *masked_entry)
{
	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	priv->mask = g_strdup ("");
	priv->tab_inside = FALSE;

	g_signal_connect (G_OBJECT (masked_entry), "key-press-event",
	                  G_CALLBACK (gtk_masked_entry_key_press_event), (gpointer)masked_entry);

	g_signal_connect (G_OBJECT (masked_entry), "delete-text",
	                  G_CALLBACK (gtk_masked_entry_delete_text), (gpointer)masked_entry);

	g_signal_connect (G_OBJECT (masked_entry), "insert-text",
	                  G_CALLBACK (gtk_masked_entry_insert_text), (gpointer)masked_entry);

	gtk_widget_set_events ((GtkWidget *)masked_entry, GDK_KEY_PRESS_MASK);
}

/**
 * gtk_masked_entry_new:
 *
 * Creates a new #GtkMaskedEntry widget.
 *
 * Returns: the newly created #GtkMaskedEntry widget.
 */
GtkWidget*
gtk_masked_entry_new ()
{
	return gtk_masked_entry_new_with_mask (NULL);
}

/**
 * gtk_masked_entry_new_with_mask:
 * @mask: the mask.
 *
 * Creates a new #GtkMaskedEntry widget with a setted mask.
 * Characters in the mask are of two types:
 *   writeable: writeable characters are characters that will be replaced with
 *              underscores and where you can enter text.
 *   fixed: every other characters are fixed characters, where you can't enter text.
 * Possible values for writeable characters are:
 *   '0': digits
 *   '9': digits excluded 0
 *   '@': alpha
 *   '^': alpha converted to upper case
 *   '#': alphanumeric
 *
 * Returns: the newly created #GtkMaskedEntry widget.
 */
GtkWidget*
gtk_masked_entry_new_with_mask (const gchar *mask)
{
	GtkWidget *masked_entry = GTK_WIDGET (g_object_new (gtk_masked_entry_get_type (), NULL));

	gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (masked_entry), mask);

	return masked_entry;
}

/**
 * gtk_masked_entry_set_mask:
 * @masked_entry: a #GtkMaskedEntry.
 * @mask: the mask (see gtk_masked_entry_new_with_mask() for explanation).
 *
 * Set the mask. View gtk_masked_entry_new_with_mask() for possible values.
 */
void
gtk_masked_entry_set_mask (GtkMaskedEntry *masked_entry,
                           const gchar    *mask)
{
	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	gint l = 0;

	if (mask != NULL && (l = strlen (mask)) != 0)
		{
			priv->mask = g_strdup (mask);
		}
	else
		{
			priv->mask = g_strdup ("");
		}

	gtk_entry_set_max_length (GTK_ENTRY (masked_entry), l);

	g_signal_emit_by_name (G_OBJECT (masked_entry), "delete-text",
	                       0, l,
	                       (gpointer)masked_entry);
}

/**
 * gtk_masked_entry_get_mask:
 * @masked_entry: a #GtkMaskedEntry.
 *
 * Returns: the mask as a string.
 */
G_CONST_RETURN gchar*
gtk_masked_entry_get_mask (GtkMaskedEntry *masked_entry)
{
	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	return g_strdup (priv->mask);
}

/**
 * gtk_masked_entry_get_text:
 * @masked_entry: a #GtkMaskedEntry.
 *
 * Returns: a pointer to the contents of the widget as a string without 
 * the mask.
 */
G_CONST_RETURN gchar*
gtk_masked_entry_get_text (GtkMaskedEntry *masked_entry)
{
	G_CONST_RETURN gchar *text, *mask;
	gchar *ret;
	gint i, c = 0, lmask;

	text = gtk_entry_get_text (GTK_ENTRY (masked_entry));
	mask = gtk_masked_entry_get_mask (masked_entry);
	lmask = strlen (mask);

	ret = g_new (gchar, lmask);
	for (i = 0; i < lmask; i++)
		{
			if (gtk_masked_entry_is_writeable (mask[i]))
				{
					if (text[i] != '_')
						{
							ret[c++] = text[i];
						}
				}
		}
	ret[c] = '\0';

	return ret;
}

/*
 * callbacks
 */
static gboolean
gtk_masked_entry_key_press_event (GtkWidget   *widget,
                                  GdkEventKey *event,
                                  gpointer     user_data)
{
	GtkMaskedEntry *masked_entry = GTK_MASKED_ENTRY (user_data);
	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	if ((event->keyval == GDK_Tab || event->keyval == GDK_ISO_Left_Tab)
		&& priv->tab_inside)
		{
			gint pos;
			GtkMaskedEntryDirections direction;

			if (event->state & GDK_SHIFT_MASK)
				{
					direction = GTK_MASKED_ENTRY_LEFT;
				}
			else
				{
					direction = GTK_MASKED_ENTRY_RIGHT;
				}

			pos = gtk_masked_entry_get_next_writeable_block (masked_entry, direction);

			if (pos > -1)
				{
					gtk_editable_set_position (GTK_EDITABLE (masked_entry), pos);
					return TRUE;
				}
			else
				{
					return FALSE;
				}
		}
	else
		{
			return FALSE;
		}
}

static void
gtk_masked_entry_delete_text (GtkEditable *editable,
                              gint         start_pos,
                              gint         end_pos,
                              gpointer     user_data)
{
	GtkMaskedEntry *masked_entry = GTK_MASKED_ENTRY (user_data);

	G_CONST_RETURN gchar *mask;
	gchar *newtext;
	gint i, c;

	if (start_pos < 0 || end_pos > gtk_entry_get_max_length (GTK_ENTRY (masked_entry)))
		{
			g_signal_stop_emission_by_name (editable, "delete-text");
			return;
		}

	newtext = g_new (gchar, end_pos - start_pos + 1);
	mask = gtk_masked_entry_get_mask (masked_entry);

	for (i = start_pos, c = 0; i < end_pos; i++, c++)
		{
			if (gtk_masked_entry_is_writeable (mask[i]))
				{
					newtext[c] = '_';
				}
			else
				{
					newtext[c] = mask[i];
				}
		}

	if (c > 0)
		{
			newtext[c] = '\0';
			i = start_pos;
			g_signal_handlers_block_by_func (editable, (gpointer)gtk_masked_entry_insert_text, user_data);
			g_signal_handlers_block_by_func (editable, (gpointer)gtk_masked_entry_delete_text, user_data);
			gtk_editable_delete_text (editable, start_pos, end_pos);
			gtk_editable_insert_text (editable, newtext, c, &i);
			g_signal_handlers_unblock_by_func (editable, (gpointer)gtk_masked_entry_delete_text, user_data);
			g_signal_handlers_unblock_by_func (editable, (gpointer)gtk_masked_entry_insert_text, user_data);
		}

	g_signal_stop_emission_by_name (editable, "delete-text");
	g_free ((gpointer)mask);
	g_free ((gpointer)newtext);
}

static void
gtk_masked_entry_insert_text (GtkEditable *editable,
                              const gchar *text,
                              gint         length,
                              gint        *position,
                              gpointer     user_data)
{
	G_CONST_RETURN gchar *mask;
	gchar *newtext;
	gint i, c = 0, lmask;

	GtkMaskedEntry *masked_entry = GTK_MASKED_ENTRY (user_data);

	if (text == NULL || length == 0)
		{
			gtk_editable_delete_text (editable, 0, gtk_entry_get_max_length (GTK_ENTRY (masked_entry)));
			g_signal_stop_emission_by_name (editable, "insert-text");
			return;
		}

	mask = gtk_masked_entry_get_mask (masked_entry);
	lmask = strlen (mask);
	if (lmask == 0 || *position >= lmask)
		{
			g_signal_stop_emission_by_name (editable, "insert-text");
			return;
		}

	/* get first available position that it is a writeable mask char */
	*position = gtk_masked_entry_get_first_writeable_mask (masked_entry, GTK_MASKED_ENTRY_RIGHT);
	
	newtext = g_new (gchar, MIN (length, lmask));

	/* checking if text is valid */
	for (i = 0; i < MIN (length, lmask); i++)
		{
			if (!gtk_masked_entry_is_writeable (mask[*position + i]))
				{
					newtext[c++] = mask[*position + i];
				}
			else if (isdigit (text[i]) &&
			         (mask[*position + i] == '0' || (mask[*position + i] == '9' && text[i] != '0') || mask[*position + i] == '#'))
				{
					newtext[c++] = text[i];
				}
			else if (isalpha (text[i]) &&
				  (mask[*position + i] == '@' || mask[*position + i] == '^') || mask[*position + i] == '#')
				{
					newtext[c++] = (mask[*position + i] == '^' ? toupper (text[i]) : text[i]);
				}
    }

	if (c > 0)
		{
			newtext[c] = '\0';
			g_signal_handlers_block_by_func (editable, (gpointer)gtk_masked_entry_insert_text, user_data);
			g_signal_handlers_block_by_func (editable, (gpointer)gtk_masked_entry_delete_text, user_data);
			gtk_editable_delete_text (editable, *position, *position + c);
			gtk_editable_insert_text (editable, newtext, c, position);
			g_signal_handlers_unblock_by_func (editable, (gpointer)gtk_masked_entry_delete_text, user_data);
			g_signal_handlers_unblock_by_func (editable, (gpointer)gtk_masked_entry_insert_text, user_data);
		}
  
	g_signal_stop_emission_by_name (editable, "insert-text");
	g_free ((gpointer)mask);
	g_free ((gpointer)newtext);
}

/*
 * private functions
 */
static gboolean
gtk_masked_entry_is_writeable (gchar c)
{
	switch (c)
		{
			case '0':               /* digit included 0 */
			case '9':               /* digit excluded 0 */
			case '@':               /* alpha */
			case '^':               /* alpha converted to upper case */
			case '#':               /* alphanumeric */
				return TRUE;

		  default:
			return FALSE;
		}
}

static gint
gtk_masked_entry_get_first_writeable_mask (GtkMaskedEntry *masked_entry,
                                           GtkMaskedEntryDirections direction)
{
	gint i;
	gint pos;
	gint lmask;

	pos = gtk_editable_get_position (GTK_EDITABLE (masked_entry));
	G_CONST_RETURN gchar *mask = gtk_masked_entry_get_mask (masked_entry);
	lmask = gtk_entry_get_max_length (GTK_ENTRY (masked_entry));

	for (i = pos; (direction == GTK_MASKED_ENTRY_LEFT ? i > 0 : i < lmask);
		 (direction == GTK_MASKED_ENTRY_LEFT ? i-- : i++))
		{
			if (gtk_masked_entry_is_writeable (mask[i]))
				{
					break;
				}
		}

	return i;
}

static gint
gtk_masked_entry_get_next_writeable_block (GtkMaskedEntry *masked_entry,
                                           GtkMaskedEntryDirections direction)
{
	gint i;
	gint pos;
	gint lmask;
	gboolean found = FALSE;

	pos = gtk_editable_get_position (GTK_EDITABLE (masked_entry));
	G_CONST_RETURN gchar *mask = gtk_masked_entry_get_mask (masked_entry);
	lmask = gtk_entry_get_max_length (GTK_ENTRY (masked_entry));

	if (direction == GTK_MASKED_ENTRY_LEFT)
		{
			if (pos == 0)
				{
					i = -1;
				}
			else
				{
					for (i = pos; i > 0; i--)
						{
							if (!gtk_masked_entry_is_writeable (mask[i]))
								{
									do
										{
											gtk_editable_set_position (GTK_EDITABLE (masked_entry), i);
											i = gtk_masked_entry_get_first_writeable_mask (masked_entry, direction);
										} while (gtk_masked_entry_is_writeable (mask[i]) && i > 0);
									break;
								}
						}
				}
		}
	else if (direction == GTK_MASKED_ENTRY_RIGHT)
		{
			for (i = pos; i < lmask; i++)
				{
					if (!gtk_masked_entry_is_writeable (mask[i]))
						{
							found = TRUE;
							break;
						}
				}

			if (found)
				{
					gtk_editable_set_position (GTK_EDITABLE (masked_entry), i);
					i = gtk_masked_entry_get_first_writeable_mask (masked_entry, direction);
					if (i == lmask)
						{
							i = -1;
						}
				}
			else
				{
					i = -1;
				}
		}

	return i;
}

static void
gtk_masked_entry_get_block_positions (GtkMaskedEntry *masked_entry, gint *start, gint *end)
{
	gint i;
	gint pos;
	gint lmask;

	pos = gtk_editable_get_position (GTK_EDITABLE (masked_entry));
	G_CONST_RETURN gchar *mask = gtk_masked_entry_get_mask (masked_entry);
	lmask = gtk_entry_get_max_length (GTK_ENTRY (masked_entry));

	for (i = pos; i > 0; i--)
		{
			if (!gtk_masked_entry_is_writeable (mask[i]))
				{
					break;
				}
		}
	*start = i;

	for (i = pos; i < lmask; i++)
		{
			if (!gtk_masked_entry_is_writeable (mask[i]))
				{
					break;
				}
		}
	*end = i;
}

static void
gtk_masked_entry_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GtkMaskedEntry *masked_entry = GTK_MASKED_ENTRY (object);

	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	switch (property_id)
		{
			case PROP_MASK:
				gtk_masked_entry_set_mask (masked_entry, g_value_get_string (value));
				break;

			case PROP_TAB_INSIDE:
				priv->tab_inside = g_value_get_boolean (value);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}

static void
gtk_masked_entry_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GtkMaskedEntry *masked_entry = GTK_MASKED_ENTRY (object);

	GtkMaskedEntryPrivate *priv = GTK_MASKED_ENTRY_GET_PRIVATE (masked_entry);

	switch (property_id)
		{
			case PROP_MASK:
				g_value_set_string (value, gtk_masked_entry_get_mask (masked_entry));
				break;

			case PROP_TAB_INSIDE:
				g_value_set_boolean (value, priv->tab_inside);
				break;

			default:
				G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
				break;
		}
}
