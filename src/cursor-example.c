/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2013 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU Lesser General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Author: Tristan Van Berkom <tristanvb@openismus.com>
 */
 
#include "cursor-example.h"
#include "cursor-data.h"

#define N_SLOTS 10

/* GObjectClass */
static void            cursor_example_class_init              (CursorExampleClass *klass);
static void            cursor_example_init                    (CursorExample      *example);
static void            cursor_example_dispose                 (GObject            *object);

/* Callbacks */
static gboolean        cursor_example_window_deleted          (GtkWidget          *widget,
							       GdkEvent           *event,
							       CursorExample      *example);
static void            cursor_example_alphabet_button_clicked (GtkButton          *button,
							       CursorExample      *example);
static void            cursor_example_up_button_clicked       (GtkButton          *button,
							       CursorExample      *example);
static void            cursor_example_down_button_clicked     (GtkButton          *button,
							       CursorExample      *example);

/* Utilities */
static void            cursor_example_load_alphabet           (CursorExample      *example);
static void            cursor_example_load_page               (CursorExample      *example);
static void            cursor_example_update_status           (CursorExample      *example);
static void            cursor_example_update_sensitivity      (CursorExample      *example);
static void            cursor_example_update_current_index    (CursorExample      *example,
							       EContact           *contact);


struct _CursorExamplePrivate {
  /* Screen widgets */
  GtkWidget *window;
  GtkWidget *browse_up_button;
  GtkWidget *browse_down_button;
  GtkWidget *total_label;
  GtkWidget *position_label;
  GtkWidget *alphabet_box;
  GtkWidget *alphabet_label;
  GtkWidget *slots[N_SLOTS];

  /* EDS SqliteDB and Cursor */
  EBookBackendSqliteDB *ebsdb;
  EbSdbCursor          *cursor;

  /* Keep track of how many contacts were loaded in the
   * last page refresh (helps to drive sensitivity) */
  gint       n_loaded;
};

G_DEFINE_TYPE (CursorExample, cursor_example, G_TYPE_OBJECT);

/************************************************************************
 *                          GObjectClass                                *
 ************************************************************************/
static void
cursor_example_class_init (CursorExampleClass *klass)
{
  GObjectClass       *object_class;

  object_class    = G_OBJECT_CLASS (klass);

  object_class->dispose  = cursor_example_dispose;

  g_type_class_add_private (object_class, sizeof (CursorExamplePrivate));
}

static void
load_assets (CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GtkBuilder *builder;
  GtkWidget *scrolled_window;
  GError *error = NULL;
  gchar *name;
  gint i;


  builder = gtk_builder_new ();

  if (!gtk_builder_add_from_resource (builder,
				      "/com/cursor/example/cursor-example.glade",
				      &error))
    g_error ("Failed to load interface ! aborting.");

  priv->window             = (GtkWidget *)gtk_builder_get_object (builder, "main_window");
  priv->browse_up_button   = (GtkWidget *)gtk_builder_get_object (builder, "browse_up_button");
  priv->browse_down_button = (GtkWidget *)gtk_builder_get_object (builder, "browse_down_button");
  priv->alphabet_box       = (GtkWidget *)gtk_builder_get_object (builder, "alphabet_box");
  priv->alphabet_label     = (GtkWidget *)gtk_builder_get_object (builder, "alphabet_label");
  priv->total_label        = (GtkWidget *)gtk_builder_get_object (builder, "total_label");
  priv->position_label     = (GtkWidget *)gtk_builder_get_object (builder, "position_label");

  g_assert (priv->window);
  g_assert (priv->browse_up_button);
  g_assert (priv->browse_down_button);
  g_assert (priv->alphabet_box);
  g_assert (priv->alphabet_label);
  g_assert (priv->total_label);
  g_assert (priv->position_label);

  for (i = 0; i < N_SLOTS; i++)
    {
      name = g_strdup_printf ("contact_slot_%d", i + 1);
      priv->slots[i] = (GtkWidget *)gtk_builder_get_object (builder, name);
      g_assert (priv->slots[i]);
      g_free (name);
    }

  g_signal_connect (priv->window, "delete-event",
		    G_CALLBACK (cursor_example_window_deleted), NULL);
  g_signal_connect (priv->browse_up_button, "clicked",
		    G_CALLBACK (cursor_example_up_button_clicked), example);
  g_signal_connect (priv->browse_down_button, "clicked",
		    G_CALLBACK (cursor_example_down_button_clicked), example);

  /* Make focus drive the scrollbar */
  scrolled_window = (GtkWidget *)gtk_builder_get_object (builder, "scrolled_window");
  g_assert (scrolled_window);

  gtk_container_set_focus_vadjustment (GTK_CONTAINER (priv->alphabet_box),
				       gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (scrolled_window)));

  g_object_unref (builder);
}

static void
cursor_example_init (CursorExample *example)
{
  example->priv = 
    G_TYPE_INSTANCE_GET_PRIVATE (example,
				 CURSOR_TYPE_EXAMPLE,
				 CursorExamplePrivate);

  load_assets (example);
}

static void
cursor_example_dispose (GObject  *object)
{
  CursorExample        *example = CURSOR_EXAMPLE (object);
  CursorExamplePrivate *priv = example->priv;

  if (priv->window)
    {
      gtk_widget_destroy (priv->window);
      priv->window = NULL;
    }

  if (priv->ebsdb)
    {
      e_book_backend_sqlitedb_cursor_free (priv->ebsdb, priv->cursor);
      g_object_unref (priv->ebsdb);

      priv->ebsdb = NULL;
      priv->cursor = NULL;
    }

  G_OBJECT_CLASS (cursor_example_parent_class)->dispose (object);
}


/************************************************************************
 *                             Callbacks                                *
 ************************************************************************/
static gboolean
cursor_example_window_deleted (GtkWidget          *widget,
			       GdkEvent           *event,
			       CursorExample      *example)
{
  /* Quit the app when the window is deleted */
  gtk_main_quit ();

  return TRUE;
}

static void
cursor_example_up_button_clicked (GtkButton     *button,
				  CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GError               *error = NULL;

  g_print ("Browse up one clicked\n");

  /* Move the cursor back N_SLOTS + 1 */
  if (!e_book_backend_sqlitedb_cursor_move_by (priv->ebsdb, priv->cursor,
  					       EBSDB_CURSOR_ORIGIN_CURRENT,
					       0 - (N_SLOTS + 1),
					       NULL, &error))
    {
      g_warning ("Failed to move the cursor: %s", error->message);
      g_error_free (error);
    }
  else
    {
      /* And load one page full of results starting with this index */
      cursor_example_load_page (example);
      cursor_example_update_sensitivity (example);
      cursor_example_update_status (example);
    }
}

static void
cursor_example_down_button_clicked (GtkButton     *button,
				    CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GError               *error = NULL;

  g_print ("Browse down one clicked\n");

  /* Move the cursor back N_SLOTS - 1 */
  if (!e_book_backend_sqlitedb_cursor_move_by (priv->ebsdb, priv->cursor,
  					       EBSDB_CURSOR_ORIGIN_CURRENT,
					       0 - (N_SLOTS - 1),
					       NULL, &error))
    {
      g_warning ("Failed to move the cursor: %s", error->message);
      g_error_free (error);
    }
  else
    {
      /* And load one page full of results starting with this index */
      cursor_example_load_page (example);
      cursor_example_update_sensitivity (example);
      cursor_example_update_status (example);
    }
}

static void
cursor_example_alphabet_button_clicked (GtkButton     *button,
					CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  gint                  index;

  index = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "alphabetic-index"));

  g_print ("Alphabet index clicked: %d\n", index);

  /* Move to this index */
  e_book_backend_sqlitedb_cursor_set_target_alphabetic_index (priv->ebsdb, priv->cursor, index);

  /* And load one page full of results starting with this index */
  cursor_example_load_page (example);
  cursor_example_update_sensitivity (example);
  cursor_example_update_status (example);
}

/************************************************************************
 *                             Utilities                                *
 ************************************************************************/
static void
cursor_example_load_alphabet (CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  ECollator            *collator;
  gint                  i, n_labels = 0;
  const gchar *const   *labels;
  GList                *children, *l;

  /* Incase we reuse this function for locale changes, let's destroy our
   * previously build alphabet
   */
  children = gtk_container_get_children (GTK_CONTAINER (priv->alphabet_box));
  for (l = children; l; l = l->next)
    {
      GtkWidget *child = l->data;

      gtk_widget_destroy (child);
    }
  g_list_free (children);

  /* Now lets load up the alphabet buttons
   */
  collator = e_book_backend_sqlitedb_ref_collator (priv->ebsdb);

  labels = e_collator_get_index_labels (collator, &n_labels, NULL, NULL, NULL);
  for (i = 0; i < n_labels; i++)
    {
      GtkWidget *button;

      /* Each button shows the label from the active alphabet index.
       *
       * We also use object data to store the numeric index into the alphabet
       * which this button represents.
       */
      button = gtk_button_new_with_label (labels[i]);
      gtk_button_set_alignment (GTK_BUTTON (button), 0.0, 0.5);
      gtk_widget_show (button);

      g_object_set_data (G_OBJECT (button), "alphabetic-index", GINT_TO_POINTER (i));

      /* Attach our callback to be called whenever an alphabet index button is clicked */
      g_signal_connect (button, "clicked",
			G_CALLBACK (cursor_example_alphabet_button_clicked), example);

      gtk_container_add (GTK_CONTAINER (priv->alphabet_box), button);
    }

  e_collator_unref (collator);
}

/* Returns whether a full page was loaded or if we reached 
 * the end of the results
 */
static void
cursor_example_load_page (CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GError               *error = NULL;
  GSList               *results = NULL;

  if (!e_book_backend_sqlitedb_cursor_move_by (priv->ebsdb, priv->cursor,
					       EBSDB_CURSOR_ORIGIN_CURRENT,
					       N_SLOTS,
					       &results,
					       &error))
    {
      g_warning ("Failed to fetch results from cursor: %s", error->message);
      g_error_free (error);

      priv->n_loaded = 0;
    }
  else
    {
      EbSdbSearchData *result;
      gint             i;

      priv->n_loaded = g_slist_length (results);

      /* Fill the page with results for the current cursor position
       */
      for (i = 0; i < N_SLOTS; i++)
	{
	  result = g_slist_nth_data (results, i);

	  if (result)
	    {
	      EContact *contact;
	      const gchar *family_name, *given_name;
	      gchar *txt;

	      contact = e_contact_new_from_vcard (result->vcard);

	      family_name = (const gchar *)e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);
	      given_name = (const gchar *)e_contact_get_const (contact, E_CONTACT_GIVEN_NAME);

	      txt = g_strdup_printf ("%s, %s", family_name, given_name);
	      gtk_label_set_text (GTK_LABEL (priv->slots[i]), txt);
	      g_free (txt);

	      /* For the first contact, give some visual feedback about where we
	       * are in the list, which alphabet character we're browsing right now.
	       */
	      if (i == 0)
		cursor_example_update_current_index (example, contact);

	      g_object_unref (contact);
	    }
	  else
	    {
	      gtk_label_set_text (GTK_LABEL (priv->slots[i]), "");
	    }
	}

      g_slist_free_full (results, (GDestroyNotify)e_book_backend_sqlitedb_search_data_free);
    }
}

static void
cursor_example_update_status (CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GError               *error = NULL;
  gint                  total = -1, position = -1;
  gchar                *txt;

  if (!e_book_backend_sqlitedb_cursor_calculate (priv->ebsdb, priv->cursor,
						 &total, &position, &error))
    {
      g_warning ("Failed to fetch cursor position and total: %s", error->message);
      g_error_free (error);
    }

  /* Update total indicator label */
  txt = g_strdup_printf ("%d", total);
  gtk_label_set_text (GTK_LABEL (priv->total_label), txt);
  g_free (txt);

  /* Update position indicator label */
  txt = g_strdup_printf ("%d", position);
  gtk_label_set_text (GTK_LABEL (priv->position_label), txt);
  g_free (txt);
}

/* The 'full_page' argument represents whether we managed
 * to load a full page of results or if we reached the
 * end of the results.
 */
static void
cursor_example_update_sensitivity (CursorExample *example)
{
  CursorExamplePrivate *priv = example->priv;
  GError               *error = NULL;
  gint                  total = -1, position = -1;

  if (!e_book_backend_sqlitedb_cursor_calculate (priv->ebsdb, priv->cursor,
						 &total, &position, &error))
    {
      g_warning ("Failed to fetch cursor position and total: %s", error->message);
      g_error_free (error);
    }
  else
    {
      gboolean up_sensitive;
      gboolean down_sensitive;

      if (total <= N_SLOTS)
	{
	  /* If the amount of contacts is less than the amount of visual slots,
	   * then we cannot browse up and down
	   */
	  up_sensitive = FALSE;
	  down_sensitive = FALSE;
	}
      else
	{
	  /* The cursor is always pointing to the last contact
	   * visible in the view, so if the cursor is further
	   * than N_SLOTS we can rewind.
	   *
	   * It's possible we are at position 0 after running
	   * off the end of the results, so we check if priv->n_loaded
	   * is less than a full page and let the user click back in
	   * that case.
	   */
	  up_sensitive = 
	    (priv->n_loaded < N_SLOTS) || 
	    (position > N_SLOTS);

	  /* So long as we have not reached the last contact, we can move
	  * the cursor down through the list */
	  down_sensitive =
	    (priv->n_loaded >= N_SLOTS) &&
	    (position < total);
	}

      gtk_widget_set_sensitive (GTK_WIDGET (priv->browse_up_button), up_sensitive);
      gtk_widget_set_sensitive (GTK_WIDGET (priv->browse_down_button), down_sensitive);
    }
}

/* This is called when refreshing the window contents with
 * the first contact shown in the window.
 */
static void
cursor_example_update_current_index (CursorExample *example,
				     EContact      *contact)
{
  CursorExamplePrivate *priv = example->priv;
  ECollator            *collator;
  const gchar *const   *labels;
  const gchar          *family_name;
  gint                  index;

  collator = e_book_backend_sqlitedb_ref_collator (priv->ebsdb);

  /* Get a pointer to our label array */
  labels = e_collator_get_index_labels (collator, NULL, NULL, NULL, NULL);

  /* Our primary sort key is the family name, so we need to use
   * the family name to check what index this contact belongs in.
   */
  family_name = e_contact_get_const (contact, E_CONTACT_FAMILY_NAME);

  /* Get our index */
  index = e_collator_get_index (collator, family_name);

  /* Update the UI and refresh the current alphabet index indicator.
   *
   * The index returned by e_collator_get_index() is unconditionally
   * valid as a position into the array returned by e_collator_get_index_labels() above.
   */
  gtk_label_set_text (GTK_LABEL (priv->alphabet_label), labels[index]);

  e_collator_unref (collator);
}

/************************************************************************
 *                                API                                   *
 ************************************************************************/
CursorExample *
cursor_example_new (const gchar *vcard_path)
{
  CursorExample *example;
  CursorExamplePrivate *priv;

  example = g_object_new (CURSOR_TYPE_EXAMPLE, NULL);
  priv    = example->priv;

  priv->ebsdb = cursor_load_data (vcard_path, &priv->cursor);

  cursor_example_load_alphabet (example);

  cursor_example_load_page (example);
  cursor_example_update_sensitivity (example);
  cursor_example_update_status (example);

  return example;
}

void
cursor_example_show (CursorExample *example)
{
  CursorExamplePrivate *priv;

  g_return_if_fail (CURSOR_IS_EXAMPLE (example));

  priv = example->priv;

  gtk_widget_show (priv->window);
}
