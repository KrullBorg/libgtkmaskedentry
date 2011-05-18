/*
 * GtkCellRendererMasked widget for GTK+
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

#ifndef __GTK_CELL_RENDERER_MASKED_H__
#define __GTK_CELL_RENDERER_MASKED_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS


#define GTK_TYPE_CELL_RENDERER_MASKED                 (gtk_cell_renderer_masked_get_type ())
#define GTK_CELL_RENDERER_MASKED(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_CELL_RENDERER_MASKED, GtkCellRendererMasked))
#define GTK_CELL_RENDERER_MASKED_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_CELL_RENDERER_MASKED, GtkCellRendererMaskedClass))
#define GTK_IS_CELL_RENDERER_MASKED(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_CELL_RENDERER_MASKED))
#define GTK_IS_CELL_RENDERER_MASKED_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_CELL_RENDERER_MASKED))
#define GTK_CELL_RENDERER_MASKED_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_CELL_RENDERER_MASKED, GtkCellRendererMaskedClass))


typedef struct _GtkCellRendererMasked        GtkCellRendererMasked;
typedef struct _GtkCellRendererMaskedClass   GtkCellRendererMaskedClass;


struct _GtkCellRendererMasked
{
  GtkCellRendererText parent;
};

struct _GtkCellRendererMaskedClass
{
  GtkCellRendererTextClass parent_class;
};


GType gtk_cell_renderer_masked_get_type (void) G_GNUC_CONST;

GtkCellRenderer *gtk_cell_renderer_masked_new (void);
GtkCellRenderer *gtk_cell_renderer_masked_new_with_mask (const gchar *mask);

void gtk_cell_renderer_masked_set_mask (GtkCellRendererMasked *renderer, const gchar *mask);


G_END_DECLS


#endif /* __GTK_CELL_RENDERER_MASKED_H__ */
