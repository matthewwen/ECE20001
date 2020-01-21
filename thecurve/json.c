#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "json.h"

const int WRAPPER   = 2; // {} or "" etc
const int COMMA     = 2; // ', '
const int TRUE_LEN  = 4; // 'true'
const int FALSE_LEN = 5; // 'false'
const int COLON     = 2; // ': '
const int NULL_TERM = 1; // '\0'

json_err_t check_leakage(char * json, char * qloc);
json_err_t parse_jobject(jobject ** obj, char ** json);
json_err_t parse_str(char **str, char **json);
void write_str(jstring * jstr, const char * append);

jobject * parse_json(char * json) {
    json_err_t res = strchr(json, '{') != NULL && strchr(json, '}') != NULL
                    ? JSON_OKAY: JSON_MISLEADING_BRACKET;
    jobject * obj = NULL;
    char * minusbrack = json + 1;
    char ** pos = &minusbrack;
    for (; **pos != '}' && !res; *pos = *pos + 1) {
        if ((res = parse_jobject(&obj, pos)) != JSON_OKAY) {
            printf("There is an error %d", res);
            free_json(&obj);
            return NULL;
        }
    }
    return obj;
}

json_err_t append_jlist(jlist ** head, type_t type, data_t data) {
    jlist * nli = malloc(sizeof(*nli));
    *nli        = (jlist) {.type = type, .data = data, .next = NULL};
    jlist ** ali = head;
    for (;*ali != NULL; ali = &(*ali)->next){}
    *ali = nli;
    // -> if type != head->type (check at the end??)
    return JSON_OKAY;
}

jobject * create_jobject(char * name, type_t type, data_t data) {
    jobject * jobj = malloc(sizeof(*jobj));
    *jobj = (jobject) {.name = NULL, .type = type, .data = data};
    jobj->name = malloc((strlen(name) + 1) * sizeof(*name));
    strcpy(jobj->name, name);
    return jobj;
}

json_err_t append_jobject(jobject ** a_object, const char * key, type_t type, data_t data) {
    for (; *a_object != NULL; a_object = &(*a_object)->next) {}

    *a_object = malloc(sizeof(**a_object));
    **a_object = (jobject) {.name = NULL, .type = type, 
                            .data = data, .next = NULL};

    (*a_object)->name = malloc((strlen(key) + 1) * sizeof(*key));
    strcpy((*a_object)->name, key);

    return JSON_OKAY;
}

json_err_t data_tostring(jstring *, type_t type, data_t data);

void write_str(jstring * jstr, const char * append) {
    int is_used  = jstr->last_pos - jstr->str;
    int to_be_used = strlen(append) + 1;
    if (((jstr->alloc_size) < (is_used + to_be_used))) {
        jstr->alloc_size = jstr->alloc_size + 100 * sizeof(*jstr->str);
        jstr->str        = realloc(jstr->str, jstr->alloc_size);
        jstr->last_pos   = jstr->str + is_used;
    }
    strcpy(jstr->last_pos, append);
    jstr->last_pos = rindex(jstr->last_pos, '\0');
}

json_err_t object_tostring(jstring * jstr, jobject * object) {
    write_str(jstr, "{");
    for (;object != NULL; object = object->next) {
        write_str(jstr, "\"");
        write_str(jstr, object->name);
        write_str(jstr, "\": ");
        data_tostring(jstr, object->type, object->data);
        if (object->next != NULL) {
            write_str(jstr, ", ");
        }
    }
    write_str(jstr, "}");
    return JSON_OKAY;
}

json_err_t data_tostring(jstring * jstr, type_t type, data_t data) {
    if (type == TEXT) {
        write_str(jstr, "\"");
        write_str(jstr, data.txt);
        write_str(jstr, "\"");
    }
    else if (type == CON) {
        const char * cond = data.cond ? "true" : "false";
        write_str(jstr, cond);
    }
    else if (type == LIST) {
        write_str(jstr, "[");
        for (jlist * curr = data.list; curr != NULL; curr = curr->next) {
            data_tostring(jstr, curr->type, curr->data);
            if (curr->next != NULL) {
                write_str(jstr, ", ");
            }
        }
        write_str(jstr, "]");
    }
    else if (type == OBJ) {
        object_tostring(jstr, data.obj);
    }
    else if (type == NUM) {
        char num[10];
        sprintf(num, "%d", data.num);
        write_str(jstr, num);
    }

    return JSON_OKAY;
}

json_err_t json_tostring(char ** a_str, jobject * object) {
    int num_char = get_json_length(object);
    size_t alloc_size;
    *a_str = malloc((alloc_size = (num_char * sizeof(**a_str))));
    jstring jstr = {.alloc_size = alloc_size, 
                    .str        = (*a_str), 
                    .last_pos   = (*a_str)};
    return object_tostring(&jstr, object);
}

json_err_t parse_jobject(jobject ** obj, char ** json) {
    json_err_t res = JSON_OKAY;
    jobject * nobj = malloc(sizeof(*nobj));
    nobj->next = NULL;
    if (!(res = parse_str(&nobj->name, json))) {
        *json += 1;
        char * colon = strchr(*json, ':');

        if (!(res = check_leakage(*json, colon))) {
            *json = colon + 1;
            for (;isspace(**json); *json += 1){};
            if (**json == '\"') {
                nobj->type = TEXT;
                res = parse_str(&nobj->data.txt, json);
            }
        }

    }
    jobject ** curr = obj;
    for (; *curr != NULL; *curr = (*curr)->next) {}
    *curr = nobj;

    if (res) {
        free_json(obj);
    }
    return res;
}

bool free_json(jobject ** obj) {
    if (*obj != NULL)  {
        if ((*obj)->next != NULL) {
            free_json(&(*obj)->next);
        }
        free((*obj)->name);

        char * txt  = NULL;
        jlist * head = NULL;
        type_t type = (*obj)->type;
        if (type == TEXT && (txt = (*obj)->data.txt) != NULL) {
            free(txt);
        }
        else if (type == OBJ) {
            free_json(&(*obj)->data.obj);
        }
        else if (type == LIST && (head = (*obj)->data.list) != NULL) {
            for (jlist * curr = head; curr != NULL; curr = head) {
                head = curr->next;
                if (curr->type == TEXT && (txt = curr->data.txt) != NULL) {
                    free(txt);
                }
                else if (curr->type == OBJ) {
                    free_json(&curr->data.obj);
                }
                free(curr);
            }
        }
        free(*obj);
    }
    *obj = NULL;
    return JSON_OKAY;
}

json_err_t check_leakage(char * json, char * qloc) {
    if (qloc < json) {
        return JSON_MISLEADING_QUOT;
    }
    for (char *curr = json; curr < qloc; curr = curr + 1) {
        if (!isspace(*curr)) {
            return JSON_LEAKED;
        }
    }
    return JSON_OKAY;
}

json_err_t parse_str(char ** str, char ** json) {
    json_err_t res = JSON_OKAY;
    char *qloc = strchr(*json, '\"');
    
    if ((res = check_leakage(*json, qloc)) == JSON_OKAY) {
        char *qend = strchr(qloc + 1, '\"');
        int len = (qend - qloc) / sizeof(*qloc) - 1;

        if (len > 0) {
            *str = malloc((len + 1) * sizeof(**str));
            memset(*str, 0, (len + 1) * sizeof(**str));
            memcpy(*str, qloc + 1, len * sizeof(**str));
            *json = qend;
        }
        else {
            res = JSON_MISLEADING_QUOT;
        }
    }

    return res;
}

int get_json_length(jobject * object) {
    int size = 0;
    size += WRAPPER;
    for(;object != NULL; object = object->next) {
        size += WRAPPER + strlen(object->name) + COLON;
        size += get_element_length(object->type, object->data);
        if (object->next != NULL) {
            size += COMMA;
        }
    }
    return size + NULL_TERM;
}

int get_element_length(type_t type, data_t data) {
    int size = 0;
    if (type == TEXT) {
        size += WRAPPER + strlen(data.txt);
    }
    else if (type == NUM) {
        size += data.num < 0 ? 1: 0;
        int num = abs(data.num);
        for(;num > 0 && (++size); num /= 10){}
    }
    else if (type == CON) {
        size += data.cond ? TRUE_LEN: FALSE_LEN;
    }
    else if (type == OBJ) {
        size += get_json_length(data.obj) - NULL_TERM;
    }
    else if (type == LIST) {
        size += WRAPPER; 
        for (jlist * curr = data.list; curr != NULL; curr = curr->next) {
            size += get_element_length(curr->type, curr->data);
            size += curr->next != NULL ? COMMA: 0;
        }
    }

    return size;
}
