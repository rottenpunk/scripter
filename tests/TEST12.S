//
// Test break and continue in do and while loops...
//

extern var1;

main ()
{
   while (var1 < 10) {
      if (var1 % 3)
         break;
   }

   do {
      if (var1 % 5)
         continue;

   } while (var1 < 10);

}
