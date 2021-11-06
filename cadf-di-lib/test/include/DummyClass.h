#ifndef DUMMYCLASS_H_
#define DUMMYCLASS_H_

class DummyClass {

    public:
        DummyClass() : value(0) {
        }

        DummyClass(int val) : value(val) {
        }

        void setValue(int val) {
            value = val;
        }
        int getValue() {
            return value;
        }

    private:
        int value;
};

#endif /* DUMMYCLASS_H_ */
