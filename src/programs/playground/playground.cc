
using namespace std;

class FakeArrowBase {

};

template <typename S, typename T>
class FakeArrow : FakeArrowBase {
public:
    S input;
    T output;

    template <typename U>
    void connect(FakeArrow<T,U> downstream) {
        output = downstream.input;
    }
};

int main() {
}



