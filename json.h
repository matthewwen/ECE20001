#ifndef _JSON_H_
#define _JSON_H_
#include <stdbool.h>
#include <inttypes.h>

typedef uint8_t type_t;
enum {TEXT, NUM, CON, LIST, OBJ, FLOAT};

struct jlist;
struct jobject;

typedef union{
    char * txt;
    int    num;
    bool   cond;
    float  flt;
    struct jlist * list;
    struct jobject * obj;
}data_t;

typedef struct jlist{
    type_t type;
    data_t data;
    struct jlist * next;
}jlist;

typedef struct jobject{
    char * name;
    type_t type;
    data_t data;
    struct jobject * next;
}jobject;

typedef struct jstring{
    size_t alloc_size;
    char * str;
    char * last_pos;
}jstring;

typedef uint8_t json_err_t;

enum {
    JSON_OKAY = 0,
    JSON_MISLEADING_BRACKET = 1,
    JSON_MISLEADING_QUOT = 2,
    JSON_LEAKED = 3,
    JSON_NO_COLON = 4,
};

jobject *  parse_json(char * json);
jobject * create_jobject(char * name, type_t type, data_t data);
json_err_t append_jobject(jobject ** object, const char * key, type_t type, data_t data);
json_err_t append_jlist(jlist ** head, type_t type, data_t data);

json_err_t json_tostring(char ** a_str, jobject * object);
bool       free_json(jobject ** json);

// in development
int get_json_length(jobject * object);
int get_element_length(type_t type, data_t data);

#define OBJECT_DEFAULT() {.name = NULL, \
                          .next = NULL}

#endif
