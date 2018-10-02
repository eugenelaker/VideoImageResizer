#include <memory>
#include <vector>
#include <iostream>
 
struct foo {
    int x;
    foo(int x) : x(x) {
    }
};
 
struct noop_delete { 
    void operator()(void* p) const {
    }
};
 
foo* bar1(const std::vector<foo*>& v);
void bar2(std::shared_ptr<foo> f);
 
int main(void) {
 
    // The foo objects are managed with shared_ptr
    std::shared_ptr<foo> f0 = std::make_shared<foo>(0);
    std::shared_ptr<foo> f1 = std::make_shared<foo>(1);
    std::shared_ptr<foo> f2 = std::make_shared<foo>(2);
 
    // Call bar, that returns an element from a vector of foo*
    // (not shared_ptr's!).
    // We have to unbox all the shared_ptr's to get raw pointers
    std::vector<foo*> v = { f0.get(), f1.get(), f2.get() };
    foo* f = bar1(v);
 
    // Now we have f. Let's give it to bar2 that, unfortunately,
    // accepts only a shared_ptr.
    // We "smartly" wrap it with a BRAND NEW shared_ptr,
    // with a custom deleter that does nothing.
    std::shared_ptr<foo> f_ptr(f, noop_delete());
    bar2(f_ptr);
 
    // Or with a no-op lambda
    bar2(std::shared_ptr<foo>(bar1(v), [](void*){}));
 
    return 0;
 
}
 
// This function accepts only a vector of foo* (and you can't change it)
foo* bar1(const std::vector<foo*>& v) {
    // Just an example: imagine a more complex algorithm to
    // extract an element from the vector
    return v[0];
}
 
void bar2(std::shared_ptr<foo> f) {
    // Just an example
    f->x++;
}
