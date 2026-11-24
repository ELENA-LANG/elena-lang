<Form :Name="MainWindow" :Height="850" :Width="1024" Caption="ELENA 7.0">
   <Context :Name="glContext" :X="1" :Y="1" :Width="1024" :Height="768">
   </Context>
   <Button :Name="btnExit" :X="20" :Y="785" :Width="1000" :Height="20" Caption="Close" :onClick="&onExit">
   </Button>
</Form>
