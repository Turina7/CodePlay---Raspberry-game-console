//Funções de utilidades não implementas pelo gcc
void* memcpy(void* dest, const void* src, unsigned long n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (unsigned long i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}
