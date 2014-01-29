/*
 * GtkMaskedEntry widget test
 * Copyright (C) 2005-2014 Andrea Zagli <azagli@libero.it>
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>

#include <gtk/gtk.h>

#include <gtkmaskedentry.h>
#include <gtkcellrenderermasked.h>

enum
{
	COL_MASKED,
	COLS
};

GtkWidget *window,
          *table,
          *txtMask,
          *masked_entry,
          *txtValue,
          *txtValueNoMask,
          *txtValueNew,
          *btnSetMask,
          *btnValue,
          *btnValueNoMask,
          *btnValueNew,
          *chkTabInside,
          *scrolw,
          *list;

GtkCellRenderer *rend;
GtkListStore *store;

static void
btn_setmask_on_clicked (GtkButton *button,
                        gpointer user_data)
{
	gtk_masked_entry_set_mask (GTK_MASKED_ENTRY (masked_entry), gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (txtMask)));
	g_object_set (G_OBJECT (rend), "mask", gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (txtMask)), NULL);
}

static void
btn_value_on_clicked (GtkButton *button,
                      gpointer user_data)
{
	gtk_entry_set_text (GTK_ENTRY (txtValue), gtk_entry_get_text (GTK_ENTRY (masked_entry)));
}

static void
btn_valuenomask_on_clicked (GtkButton *button,
                            gpointer user_data)
{
	gtk_entry_set_text (GTK_ENTRY (txtValueNoMask), gtk_masked_entry_get_text (GTK_MASKED_ENTRY (masked_entry)));
}

static void
btn_valuenew_on_clicked (GtkButton *button,
                            gpointer user_data)
{
	gtk_entry_set_text (GTK_ENTRY (masked_entry), gtk_entry_get_text (GTK_ENTRY (txtValueNew)));
}

static void
chk_tab_inside_on_toggled (GtkToggleButton *button,
                           gpointer user_data)
{
	g_object_set (G_OBJECT (masked_entry),
				  "tab-inside", gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chkTabInside)),
				  NULL);
}

static void
on_cell_edited (GtkCellRendererMasked *cell_renderer,
                const gchar *path_string,
                const gchar *new_text,
                gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);

	gtk_tree_model_get_iter (GTK_TREE_MODEL (store), &iter, path);

	gtk_list_store_set (store, &iter, COL_MASKED, new_text, -1);

	gtk_tree_path_free (path);
}

int
main (int argc, char **argv)
{
	GtkWidget *label;
	GtkTreeViewColumn *col;
	GtkTreeIter iter;
	
	gtk_init (&argc, &argv);
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
	gtk_window_set_title (GTK_WINDOW (window), "GtkMaskedEntry Tests");
	
	g_signal_connect (G_OBJECT (window), "destroy",
					  G_CALLBACK (gtk_main_quit), NULL);
	
	table = gtk_table_new (7, 3, FALSE);
	gtk_container_add (GTK_CONTAINER (window), table);
	gtk_widget_show (table);
	
	label = gtk_label_new ("Mask");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, 0, 0, 3, 3);
	gtk_widget_show (label);
	
	txtMask = gtk_combo_box_text_new_with_entry ();
	gtk_table_attach (GTK_TABLE (table), txtMask, 1, 2, 0, 1, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (txtMask);
	
	/* some mask examples */
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "00/00/0000");
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "999 999 999"); /* digit excluded 0 */
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "@@@ @@@ @@@"); /* alpha */
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "^^^ ^^^ ^^^"); /* alpha to upper case */
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "ABC 00@@@00");
	gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (txtMask), "### some text");

	btnSetMask = gtk_button_new_with_label ("Set Mask");
	gtk_table_attach (GTK_TABLE (table), btnSetMask, 2, 3, 0, 1, 0, 0, 3, 3);
	gtk_widget_show (btnSetMask);
	
	g_signal_connect (G_OBJECT (btnSetMask), "clicked", G_CALLBACK (btn_setmask_on_clicked), NULL);
	
	label = gtk_label_new ("Masked Entry");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, 0, 0, 3, 3);
	gtk_widget_show (label);
	
	masked_entry = gtk_masked_entry_new ();
	gtk_table_attach (GTK_TABLE (table), masked_entry, 1, 2, 1, 2, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (masked_entry);
	
	label = gtk_label_new ("Value");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 2, 3, 0, 0, 3, 3);
	gtk_widget_show (label);
	
	txtValue = gtk_entry_new ();
	gtk_table_attach (GTK_TABLE (table), txtValue, 1, 2, 2, 3, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (txtValue);
	
	btnValue = gtk_button_new_with_label ("Get");
	gtk_table_attach (GTK_TABLE (table), btnValue, 2, 3, 2, 3, 0, 0, 3, 3);
	gtk_widget_show (btnValue);
	
	g_signal_connect (G_OBJECT (btnValue), "clicked", G_CALLBACK (btn_value_on_clicked), NULL);
	
	label = gtk_label_new ("Value without mask");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 3, 4, 0, 0, 3, 3);
	gtk_widget_show (label);
	
	txtValueNoMask = gtk_entry_new ();
	gtk_table_attach (GTK_TABLE (table), txtValueNoMask, 1, 2, 3, 4, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (txtValueNoMask);
	
	btnValueNoMask = gtk_button_new_with_label ("Get");
	gtk_table_attach (GTK_TABLE (table), btnValueNoMask, 2, 3, 3, 4, 0, 0, 3, 3);
	gtk_widget_show (btnValueNoMask);
	
	g_signal_connect (G_OBJECT (btnValueNoMask), "clicked", G_CALLBACK (btn_valuenomask_on_clicked), NULL);
	
	label = gtk_label_new ("Set value");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 4, 5, 0, 0, 3, 3);
	gtk_widget_show (label);

	txtValueNew = gtk_entry_new ();
	gtk_table_attach (GTK_TABLE (table), txtValueNew, 1, 2, 4, 5, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (txtValueNew);

	btnValueNew = gtk_button_new_with_label ("Set");
	gtk_table_attach (GTK_TABLE (table), btnValueNew, 2, 3, 4, 5, 0, 0, 3, 3);
	gtk_widget_show (btnValueNew);

	g_signal_connect (G_OBJECT (btnValueNew), "clicked", G_CALLBACK (btn_valuenew_on_clicked), NULL);

	label = gtk_label_new ("Tab Inside");
	gtk_table_attach (GTK_TABLE (table), label, 0, 1, 5, 6, 0, 0, 3, 3);
	gtk_widget_show (label);
	
	chkTabInside = gtk_check_button_new ();
	gtk_table_attach (GTK_TABLE (table), chkTabInside, 1, 2, 5, 6, GTK_EXPAND | GTK_FILL, 0, 3, 3);
	gtk_widget_show (chkTabInside);
	
	g_signal_connect (G_OBJECT (chkTabInside), "toggled", G_CALLBACK (chk_tab_inside_on_toggled), NULL);

	scrolw = gtk_scrolled_window_new (NULL, NULL);
	gtk_table_attach (GTK_TABLE (table), scrolw, 0, 3, 6, 7, GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL, 3, 3);
	gtk_widget_show (scrolw);

	store = gtk_list_store_new (COLS,
								G_TYPE_STRING,
								-1);

	list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_container_add (GTK_CONTAINER (scrolw), list);
	gtk_widget_show (list);

	rend = gtk_cell_renderer_masked_new ();
	g_object_set (rend, "editable", TRUE, NULL);
	g_signal_connect (rend, "edited",
	                  G_CALLBACK (on_cell_edited), NULL);

	col = gtk_tree_view_column_new_with_attributes ("Masked",
													rend,
													"text", COL_MASKED,
													NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (list), col);

	gtk_list_store_append (store, &iter);

	gtk_widget_show (window);

	gtk_main ();

	return 0;
}
