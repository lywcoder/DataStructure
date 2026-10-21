#ifndef illegalIndex_
#define illegalIndex_

#include <iostream>
#include <string>

using namespace std;

class illegalIndex 
{
   public:
      illegalIndex(string theMessage = "Illegal index")
            {message = theMessage;}
      void outputMessage() {cout << message << endl;}
   private:
      string message;
};

#endif
