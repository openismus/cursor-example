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
#ifndef __CURSOR_DATA_H__
#define __CURSOR_DATA_H__

#include <libedata-book/libedata-book.h>
#include <libebook/libebook.h>

#define CURSOR_DATA_SOURCE_ID "cursor-example-book"

#define SQLITEDB_EMAIL_ID    "addressbook@localbackend.com"
#define SQLITEDB_FOLDER_ID   "folder_id"
#define SQLITEDB_FOLDER_NAME "folder"

EBookClient *cursor_load_data (const gchar  *vcard_path,
			       EBookBackendSqliteDB **ret_ebsdb,
			       EbSdbCursor **ret_cursor);

#endif /* __CURSOR_DATA_H__ */
