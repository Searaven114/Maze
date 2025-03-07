#ifndef TemplatedStack_H
#define TemplatedStack_H

#include <iostream>
#include <exception>

using namespace std;

//#define DEBUG

struct Cell{

    //all walls are present in the beginning.
    int bottom = 1;
    int top = 1;
    int right = 1;
    int left = 1;

    //visit status
    bool visited_gen = false;
    bool visited_pf = false;

    //Coord representation
    int x;
    int y;

    //flags for each direction, if true, means that directional move from this cell for that direction has been attempted
    bool flag_down = false;
    bool flag_up = false;
    bool flag_right = false;
    bool flag_left = false;

    //Parameterized Constructor
    Cell (int bottomW = 1, int topW = 1, int rightW = 1, int leftW = 1, bool visitedG = false, bool visitedP = false, int posX = 0, int posY = 0, bool flagD = false, bool flagU = false, bool flagR = false, bool flagL = false)
        :
         bottom(bottomW), top(topW), right(rightW), left(leftW), visited_gen(visitedG), visited_pf(visitedP), x(posX), y(posY), flag_down(flagD), flag_up(flagU), flag_right(flagR), flag_left(flagL)
        {}
};

//Templated Stack class, acquired from slides.
template <class Object>
class Stack {

public:

    Stack(); //Constructor

    Stack(const Stack & rhs); //Copy constructor

    ~Stack(); // Destructor

    bool isEmpty() const; // Util function

    bool isFull() const; // Util function

    void makeEmpty(); // Util function

    void pop(Object &result); //mutator

    void push(const Object & x); //mutator

    Object topAndPop(); //mutator

    const Object & top() const; //Accessor

    const Stack & operator=(const Stack & rhs); //overload

private:

    struct ListNode {

        Object   element;
        ListNode *next;

        ListNode(const Object & theElement, ListNode * n = NULL) : element(theElement), next(n) {}

    };

    ListNode *topOfStack;  // list itself is the stack

};



//Construct the stack.

    template <class Object>
    Stack<Object>::Stack( )
    {
          topOfStack = NULL;
          #ifdef DEBUG
          cout << "(DEBUG) A STACK HAS BEEN INSTANTIATED VIA: DEFAULT CONSTRUCTOR\n";
          #endif
    }



 // Test if the stack is logically full.
 // Return false always, in this implementation.

    template <class Object>
    bool Stack<Object>::isFull( ) const
    {
           return false;
    }



 // if the stack is logically empty.
 // Return true if empty, false, otherwise.

    template <class Object>
    bool Stack<Object>::isEmpty( ) const
    {
          return topOfStack == NULL;
    }



// Get the most recently inserted item in the stack.
// Return the most recently inserted item in the stack
// or throw an exception if empty.

    template <class Object>
    const Object & Stack<Object>::top() const {

         if ( isEmpty( ) ){
            //throw Underflow( );
            throw underflow_error("Stack is empty");
         }

         return topOfStack->element;
    }



template <class Object>
void Stack<Object>::pop(Object &result) {
    if (isEmpty()) {
        throw underflow_error("Stack is empty");
    }

    ListNode *oldTop = topOfStack;
    result = oldTop->element; // Store the popped element in the result parameter
    topOfStack = topOfStack->next;
    delete oldTop;
}



// Insert x into the stack.

    template <class Object>
    void Stack<Object>::push( const Object & x )
    {
         topOfStack = new ListNode( x, topOfStack );
    }



template <class Object>
Object Stack<Object>::topAndPop() {
    if (isEmpty() ) {
        throw underflow_error("Stack is empty");
    }

    Object topItem = top();
    pop(topItem);
    return topItem;
}


// Make the stack logically empty.

    template <class Object>
    void Stack<Object>::makeEmpty( )
    {
        Object garbage;
        while ( ! isEmpty() )
             pop(garbage);
    }


/*
 * Deep copy.
 */
    template <class Object>
    const Stack<Object> & Stack<Object>::
    operator=( const Stack<Object> & rhs )
    {
          if ( this != &rhs )
          {
               makeEmpty();
               if ( rhs.isEmpty() )
                    return *this;

               ListNode *rptr = rhs.topOfStack;
               ListNode *ptr  = new ListNode( rptr->element );
               topOfStack = ptr;

               for ( rptr = rptr->next; rptr != NULL; rptr = rptr->next )
                     ptr = ptr->next = new ListNode( rptr->element );
        }
        return *this;
    }


//Copy constructor.

    template <class Object>
    Stack<Object>::Stack( const Stack<Object> & rhs )
    {
        topOfStack = NULL;
        *this = rhs; // deep copy

        #ifdef DEBUG
        cout << "(DEBUG) A STACK HAS BEEN INSTANTIATED VIA: COPY CONSTRUCTOR\n";
        #endif
    }



//Destructor

    template <class Object>
    Stack<Object>::~Stack()
    {
        makeEmpty();

        #ifdef DEBUG
        cout << "(DEBUG) DESTRUCTOR INVOKED.\n";
        #endif
    }


#endif




