#include <Python.h>
#include <marshal.h>
#include <opcode.h>  // 用於操作碼
// #include <internal/pycore_opcode_metadata.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "op.h"

#define HEADER_SIZE 16  // Python 3.12 header size

void dump_code_object(PyCodeObject *code_obj) {
    printf("Disassembling code object: %s\n", PyUnicode_AsUTF8(code_obj->co_name));
    printf("Filename: %s\n", PyUnicode_AsUTF8(code_obj->co_filename));
    printf("First Line Number: %d\n", code_obj->co_firstlineno);
    
    // Print constants
    printf("Constants:\n");
    for (Py_ssize_t i = 0; i < PyTuple_Size(code_obj->co_consts); i++) {
        PyObject *const_obj = PyTuple_GetItem(code_obj->co_consts, i);
        printf("  [%zd] %s\n", i, PyUnicode_Check(const_obj) ? 
             PyUnicode_AsUTF8(const_obj) : "Non-string constant");
        /*
        if (PyUnicode_Check(const_obj))
            printf("  [%zd] %s\n", i,  PyUnicode_AsUTF8(const_obj));
        else
            printf("  [%zd] %d\n", i,  *(int*)const_obj);
        */
    }
    
    // Print names
    printf("Names:\n");
    for (Py_ssize_t i = 0; i < PyTuple_Size(code_obj->co_names); i++) {
        PyObject *name_item = PyTuple_GetItem(code_obj->co_names, i);
        printf("  [%zd] %s\n", i, PyUnicode_AsUTF8(name_item));
    }

    // Print variable names
    printf("Variable Names:\n");
    for (Py_ssize_t i = 0; i < PyTuple_Size(code_obj->co_names); i++) {
        PyObject *var_item = PyTuple_GetItem(code_obj->co_names, i);
        printf("  [%zd] %s\n", i, PyUnicode_AsUTF8(var_item));
    }
// 這段有錯
    // Print the bytecode
    printf("Bytecode:\n");
    PyObject *bytecode = PyCode_GetCode(code_obj); // (PyObject *) code_obj->co_code_adaptive;
    Py_ssize_t bytecode_size = PyBytes_Size(bytecode);
    unsigned char *code = (unsigned char *)PyBytes_AsString(bytecode);
    for (Py_ssize_t i = 0; i < bytecode_size; ) {
        Py_ssize_t addr = i;
        int opcode = code[i++];
        int arg_count = code[i++];
        printf("%4zd: %s %d", addr, op_names[opcode], arg_count);  // 使用 Python 的操作碼名稱 (這個在 internal/pycore_opcode_metadata 引用不到)
        if (opcode == CALL) i+=6; // CALL 指令占 8byte (不知為何？)

        printf("\n");
        // i++;
    }
}

PyCodeObject* load_code_object(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Error opening file");
        return NULL;
    }

    // Read header
    unsigned char header[HEADER_SIZE];
    fread(header, sizeof(unsigned char), HEADER_SIZE, f);

    // Read the code object using marshal
    PyObject *code_obj = PyMarshal_ReadObjectFromFile(f);
    fclose(f);
    
    if (!code_obj || !PyCode_Check(code_obj)) {
        fprintf(stderr, "Failed to read a code object.\n");
        return NULL;
    }

    return (PyCodeObject *)code_obj;
}


void run_code_object(PyCodeObject *code_obj) {
    printf("Disassembling code object: %s\n", PyUnicode_AsUTF8(code_obj->co_name));
    printf("Filename: %s\n", PyUnicode_AsUTF8(code_obj->co_filename));
    printf("First Line Number: %d\n", code_obj->co_firstlineno);

    Py_ssize_t size;
    size = PyTuple_Size(code_obj->co_consts);
    PyObject **constant_objs = malloc(sizeof(PyObject*)*size);
    // Print constants
    printf("Constants:\n");
    for (Py_ssize_t i = 0; i < PyTuple_Size(code_obj->co_consts); i++) {
        PyObject *const_obj = PyTuple_GetItem(code_obj->co_consts, i);
        constant_objs[i] = const_obj;
        // printf("  [%zd] %s", i, PyUnicode_Check(const_obj) ? 
        //      PyUnicode_AsUTF8(const_obj) : "Non-string constant");
        PyTypeObject *type = Py_TYPE(const_obj);
        printf("=>  type:%s ", type->tp_name);
        if (strcmp(type->tp_name, "int")==0) {
            long ivalue = PyLong_AsLong(const_obj);
            printf("value:%ld ", ivalue);
        } else if (strcmp(type->tp_name, "float")==0) {
            double fvalue = PyFloat_AsDouble(const_obj);
            printf("value:%f ", fvalue);
        } else if (strcmp(type->tp_name, "str")==0) {
            const char *svalue = PyUnicode_AsUTF8(const_obj);
            printf("value:%s ", svalue);
        }
        printf("\n");
    }
    
    size = PyTuple_Size(code_obj->co_names);
    const char **names = malloc(sizeof(char*)*size);
    // Print names
    printf("Names:\n");
    for (Py_ssize_t i = 0; i < PyTuple_Size(code_obj->co_names); i++) {
        PyObject *name_item = PyTuple_GetItem(code_obj->co_names, i);
        printf("  [%zd] %s\n", i, PyUnicode_AsUTF8(name_item));
        names[i] = PyUnicode_AsUTF8(name_item);
    }

    // Print the bytecode
    printf("Bytecode:\n");
    PyObject *bytecode = PyCode_GetCode(code_obj); // (PyObject *) code_obj->co_code_adaptive;
    Py_ssize_t bytecode_size = PyBytes_Size(bytecode);
    unsigned char *code = (unsigned char *)PyBytes_AsString(bytecode);
    for (Py_ssize_t i = 0; i < bytecode_size; ) {
        Py_ssize_t addr = i;
        int opcode = code[i++];
        int arg_count = code[i++];
        printf("%4zd: %s %d", addr, op_names[opcode], arg_count);  // 使用 Python 的操作碼名稱 (這個在 internal/pycore_opcode_metadata 引用不到)
        if (opcode == CALL) i+=6; // CALL 指令占 8byte (不知為何？)

        printf("\n");
        // i++;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <pyc file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize Python interpreter
    Py_Initialize();

    PyCodeObject *code_obj = load_code_object(argv[1]);
    if (code_obj) {
        dump_code_object(code_obj);
        run_code_object(code_obj);
        Py_DECREF(code_obj);  // Decrease reference count
    }

    // Finalize Python interpreter
    Py_Finalize();
    return EXIT_SUCCESS;
}
