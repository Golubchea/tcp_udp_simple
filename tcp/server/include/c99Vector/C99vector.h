#ifndef C99VECTOR_H
#define C99VECTOR_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct C99vector_ {
    void** data;
    int size;
    int count;
} C99vector;

void C99vector_init(C99vector*);
int C99vector_count(C99vector*);
void C99vector_add(C99vector*, void*);
void C99vector_set(C99vector*, int, void*);
void *C99vector_get(C99vector*, int);
void C99vector_delete(C99vector*, int);
void C99vector_free(C99vector*);


#ifdef __cplusplus
}
#endif



#endif
