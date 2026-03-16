<Form :Name="MainWindow" :Height="110" :Width="200" Caption="ELENA 6.0">
   <Label :Name="labCaption" :X="40" :Y="10" :Width="150" :Height="30" Caption="ELENA">
   </Label>
   <Button :Name="btnExit" :X="20" :Y="40" :Width="150" :Height="30" Caption="Close" :onClick="&onExit">
   </Button>
</Form>