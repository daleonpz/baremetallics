struct foo{
    int a;
    char b;
};

// foo1 is a struct foo
typedef struct foo foo1;


int main(){
    foo a;  // illegal
// there is not 'foo' type
struct foo b; // good
foo1 c; // good
 struct foo1 d; // illegal
}
