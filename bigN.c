typedef struct bigN {
    char *value;
    uint64_t size;
} bigN;

#define XOR_SWAP(a, b, type) \
    do {                     \
        type *__c = (a);     \
        type *__d = (b);     \
        *__c ^= *__d;        \
        *__d ^= *__c;        \
        *__c ^= *__d;        \
    } while (0)

static void __swap(void *a, void *b, size_t size)
{
    if (a == b)
        return;

    switch (size) {
    case 1:
        XOR_SWAP(a, b, char);
        break;
    case 2:
        XOR_SWAP(a, b, short);
        break;
    case 4:
        XOR_SWAP(a, b, unsigned int);
        break;
    case 8:
        XOR_SWAP(a, b, unsigned long);
        break;
    default:
        /* Do nothing */
        break;
    }
}

static void reverse_str(char *str, size_t n)
{
    for (int i = 0; i < (n >> 1); i++)
        __swap(&str[i], &str[n - i - 1], sizeof(char));
}

static void bigN_add(struct bigN *a, struct bigN *b, struct bigN *out)
{
    /*
     * Make sure the string length of 'a' is always greater than
     * the one of 'b'.
     */
    char *data_a, *data_b, *data_out;
    uint64_t size_a, size_b;

    size_a = a->size;
    size_b = b->size;
    if (size_a < size_b)
        __swap((void *) &a, (void *) &b, sizeof(void *));

    data_a = a->value;
    data_b = b->value;
    out->value = kmalloc(sizeof(char) * (size_a + 2), GFP_KERNEL);
    data_out = out->value;

    reverse_str(data_a, size_a);
    reverse_str(data_b, size_b);

    // smallest case digits: a = 1, b = 1, carry = 0, sum = 2
    // biggest case digits: a = 9, b = 9, carry = 1, sum = 19
    // is it possible to not use modulo and division here?
    int i, carry = 0;
    int sum;
    data_out[0] = '\0';
    for (i = 1; i < size_b; i++) {
        sum = (data_a[i] - '0') + (data_b[i] - '0') + carry;
        data_out[i] = '0' + sum % 10;
        carry = sum / 10;
    }

    for (i = size_b; i < size_a; i++) {
        sum = (data_a[i] - '0') + carry;
        data_out[i] = '0' + sum % 10;
        carry = sum / 10;
    }

    if (carry)
        data_out[i++] = '0' + carry;

    data_out[i] = 0;
    out->size = i - 1;
    reverse_str(data_out, i);

    /* Restore the original string */
    reverse_str(data_a, size_a);
    reverse_str(data_b, size_b);
    /*
    if (out)
        *out = *xs_tmp(buf);
    */
}
