/*
 * GtkMaskedEntry widget for GTK+
 * Copyright (C) 2005-2007 Andrea Zagli <azagli@libero.it>
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

#ifndef __GTK_MASKED_ENTRY_H__
#define __GTK_MASKED_ENTRY_H__

#include <gdk/gdk.h>
#include <gtk/gtkentry.h>


G_BEGIN_DECLS


#define GTK_TYPE_MASKED_ENTRY                 (gtk_masked_entry_get_type ())
#define GTK_MASKED_ENTRY(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_MASKED_ENTRY, GtkMaskedEntry))
#define GTK_MASKED_ENTRY_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_MASKED_ENTRY, GtkMaskedEntry))
#define GTK_IS_MASKED_ENTRY(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_MASKED_ENTRY))
#define GTK_IS_MASKED_ENTRY_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_MASKED_ENTRY))
#define GTK_MASKED_ENTRY_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_MASKED_ENTRY, GtkMaskedEntry))


typedef struct _GtkMaskedEntry        GtkMaskedEntry;
typedef struct _GtkMaskedEntryClass   GtkMaskedEntryClass;


struct _GtkMaskedEntry
{
	GtkEntry entry;
};

struct _GtkMaskedEntryClass
{
	GtkEntryClass parent_class;
};


GType gtk_masked_entry_get_type (void) G_GNUC_CONST;

GtkWidget *gtk_masked_entry_new (void);
GtkWidget *gtk_masked_entry_new_with_mask (const gchar *mask);

void gtk_masked_entry_set_mask (GtkMaskedEntry *masked_entry,
                                const gchar *mask);

G_CONST_RETURN gchar *gtk_masked_entry_get_mask (GtkMaskedEntry *masked_entry);
G_CONST_RETURN gchar *gtk_masked_entry_get_text (GtkMaskedEntry *masked_entry);


G_END_DECLS


#endif /* __GTK_MASKED_ENTRY_H__ */
