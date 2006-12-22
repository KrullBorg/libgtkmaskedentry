/*
 * GtkMaskedEntry widget for GTK+
 * Copyright (C) 2005-2006 Andrea Zagli <azagli@libero.it>
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

#include <ctype.h>
#include <string.h>

#include <glib/gprintf.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkentry.h>

#include "gtkmaskedentry.h"

typedef enum
{
  GTK_MASKED_ENTRY_LEFT   = -1,
  GTK_MASKED_ENTRY_RIGHT  = 1
} GtkMaskedEntryDirections;

static void     gtk_masked_entry_class_init   (GtkMaskedEntryClass *klass);
static void     gtk_masked_entry_init         (GtkMaskedEntry *masked_entry);

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
static gint     gtk_masked_entry_get_first_writeable_mask (GtkMaskedEntry *masked_entry,
                                                           GtkMaskedEntryDirections direction);

static GtkWidgetClass *parent_class = NULL;

GType
gtk_masked_entry_get_type (void)
{
  static GType masked_entry_type = 0;

  if (!masked_entry_type)
    {
      static const GTypeInfo masked_entry_info =
      {
        sizeof (GtkMaskedEntryClass),
        NULL,		/* base_init */
        NULL,		/* base_finalize */
        (GClassInitFunc) gtk_masked_entry_class_init,
        NULL,		/* class_finalize */
        NULL,		/* class_data */
        sizeof (GtkMaskedEntry),
        0,		/* n_preallocs */
        (GInstanceInitFunc) gtk_masked_entry_init,
      };

      masked_entry_type = g_type_register_static (GTK_TYPE_ENTRY, "GtkMaskedEntry",
				                                          &masked_entry_info, 0);
    }

  return masked_entry_type;
}

static void
gtk_masked_entry_class_init (GtkMaskedEntryClass *class)
{
  GtkWidgetClass *widget_class;

  widget_class = (GtkWidgetClass*) class;
  parent_class = g_type_class_peek_parent (class);
}

static void
gtk_masked_entry_init (GtkMaskedEntry *masked_entry)
{
  masked_entry->mask = g_strdup ("");

  g_signal_connect (G_OBJECT (masked_entry), "delete-text",
                    G_CALLBACK (gtk_masked_entry_delete_text), (gpointer)masked_entry);

  g_signal_connect (G_OBJECT (masked_entry), "insert-text",
                    G_CALLBACK (gtk_masked_entry_insert_text), (gpointer)masked_entry);
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
 *              and underscore and where you can enter text.
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
 * @mask: the mask.
 *
 * Set the mask. View gtk_masked_entry_new_with_mask() for possible values.
 */
void
gtk_masked_entry_set_mask (GtkMaskedEntry *masked_entry,
                           const gchar    *mask)
{
  gint l = 0;

  if (mask != NULL && (l = strlen (mask)) != 0)
    {
      masked_entry->mask = g_strdup (mask);
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
  return g_strdup (masked_entry->mask);
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
  newtext[c] = '\0';

  if (c > 0)
    {
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
	newtext[c] = '\0';

	if (c > 0)
		{
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
  gint i, pos, lmask;

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
