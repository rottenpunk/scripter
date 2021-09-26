//
// Test switch statement...
//

routine1(var1, var2)
{

   switch (var1) {
      case 1:
         var2 = "Test1";
         break;
      case 2:
         while (var2 == "Test2") {
            if (var2 == "Test1")
               break;
         }
         break;
      default:
         var3 = "Test3";
   }

   switch (var1 + 10) {
      case 127:
         var1 = 10;
      case 129:
         var2 = 11;
   }

}
