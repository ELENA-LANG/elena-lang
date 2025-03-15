<Form :Name="MainWindow" :Height="200" :Width="200" Caption="ELENA 6.0">
   <Label :Name="labCaption" :X="10" :Y="10" :Width="100" :Height="15" Caption="Hello World">
   </Label>
   <Button :Name="btnExit" :X="110" :Y="8" :Width="70" :Height="25" Caption="Exit" :onClick="&onExit">
   </Button>
</Form>