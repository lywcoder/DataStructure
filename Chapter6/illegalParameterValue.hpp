#ifndef illegalParameterValue_
#define illegalParameterValue_

#include <iostream>
#include <string>

using namespace std;

class illegalParameterValue 
{
   public:
      illegalParameterValue(string theMessage = "Illegal parameter value")
            {message = theMessage;}
      void outputMessage() {cout << message << endl;}
   private:
      string message;
};

#endif
