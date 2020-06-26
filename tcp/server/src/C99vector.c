#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <c99Vector/C99vector.h>

void C99vector_init(C99vector *v)
{
    v->data = NULL;
    v->size = 0;
    v->count = 0;
}

int C99vector_count(C99vector *v)
{
    return v->count;
}

void C99vector_add(C99vector *v, void *e)
{
    if (v->size == 0) {
        v->size = 10;
        v->data = malloc(sizeof(void*) * v->size);
        memset(v->data, '\0', sizeof(void*) * v->size);
    }

    if (v->size == v->count) {
        v->size *= 2;
        v->data = realloc(v->data, sizeof(void*) * v->size);
    }

    v->data[v->count] = e;
    v->count++;
}

void C99vector_set(C99vector *v, int index, void *e)
{
    if (index >= v->count) {
        return;
    }

    v->data[index] = e;
}

void *C99vector_get(C99vector *v, int index)
{
    if (index >= v->count) {
        return NULL;
    }

    return v->data[index];
}

void C99vector_delete(C99vector *v, int index)
{
    if (index >= v->count) {
        return;
    }

    for (int i = index, j = index; i < v->count; i++) {
        v->data[j] = v->data[i];
        j++;
    }

    v->count--;
}

void C99vector_free(C99vector *v)
{
    free(v->data);
}

//int main(void)
//{
//    C99vector v;
//    C99vector_init(&v);

//    C99vector_add(&v, "emil");
//    C99vector_add(&v, "hannes");
//    C99vector_add(&v, "lydia");
//    C99vector_add(&v, "olle");
//    C99vector_add(&v, "erik");

//    int i;
//    printf("first round:\n");
//    for (i = 0; i < C99vector_count(&v); i++) {
//        printf("%s\n", (char*)C99vector_get(&v, i));
//    }

//    C99vector_delete(&v, 1);
//    C99vector_delete(&v, 3);

//    printf("second round:\n");
//    for (i = 0; i < C99vector_count(&v); i++) {
//        printf("%s\n", (char*)C99vector_get(&v, i));
//    }

//    C99vector_free(&v);

//    return 0;
//}
