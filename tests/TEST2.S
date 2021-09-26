main()
{

   Input = receive(Session);
   if (strcmp(Input, "Test") == 0) {
      send(Session, "Test1");
   }


}

