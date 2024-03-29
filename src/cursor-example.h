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
 
#ifndef __CURSOR_EXAMPLE_H__
#define __CURSOR_EXAMPLE_H__

#include <libedata-book/libedata-book.h>
#include <libedataserver/libedataserver.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define CURSOR_TYPE_EXAMPLE            (cursor_example_get_type())
#define CURSOR_EXAMPLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CURSOR_TYPE_EXAMPLE, CursorExample))
#define CURSOR_EXAMPLE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CURSOR_TYPE_EXAMPLE, CursorExampleClass))
#define CURSOR_IS_EXAMPLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CURSOR_TYPE_EXAMPLE))
#define CURSOR_IS_EXAMPLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CURSOR_TYPE_EXAMPLE))
#define CURSOR_EXAMPLE_GET_CLASS(o)    (G_TYPE_INSTANCE_GET_CLASS ((o), CURSOR_EXAMPLE, CursorExampleClass))

typedef struct _CursorExample         CursorExample;
typedef struct _CursorExamplePrivate  CursorExamplePrivate;
typedef struct _CursorExampleClass    CursorExampleClass;

struct _CursorExample
{
  GObject parent_instance;

  CursorExamplePrivate *priv;
};

struct _CursorExampleClass
{
  GObjectClass parent_class;
};

GType              cursor_example_get_type             (void) G_GNUC_CONST;
CursorExample     *cursor_example_new                  (const gchar   *vcard_path);
void               cursor_example_show                 (CursorExample *example);

G_END_DECLS

#endif /* __CURSOR_EXAMPLE_H__ */
