#pragma once

static int countdown_min;
static int countdown_sec;
char* str_countdown_time;

static TextLayer *countdown_layer;
static AppTimer *countdown_timer;

void countdown_callback(void*);