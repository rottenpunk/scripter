//-----------------------------------------------------------------------------
//   Demonstration program for
//
//
//-----------------------------------------------------------------------------

#include "test1.sh"

main(argc, argv)
{

   while ((Input = receive(Session)) != NULL) {

      if (strncmp(Input, "Input", CONSTANT1) == 0)
         goto label1;
      else {
         send(Session, CONSTANT2);
         return;
      }
   }

label1:

   for (i = 0; i < 10; i++) {
      var1 = i + var2 + var3 / var4 + (var5 + (var6 + (var7 * var8)));
   }
}

send(var1, var2)
{
   return var1 += var2;
}
