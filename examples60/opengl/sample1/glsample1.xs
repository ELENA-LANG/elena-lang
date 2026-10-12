<Form :Name="MainWindow" :Height="220" :Width="200" Caption="ELENA 7.0">
   <Context :Name="glContext" :X="1" :Y="1" :Width="200" :Height="150">
   </Context>
   <Button :Name="btnExit" :X="20" :Y="160" :Width="150" :Height="30" Caption="Close" :onClick="&onExit">
   </Button>
</Form>
