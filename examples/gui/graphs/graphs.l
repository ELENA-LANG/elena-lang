import system'math;
import forms;
import system'drawing;
import extensions;

const  int ScaleX   = 15;
const  int ScaleY   = 15;
const  real starting = 0.0r;
symbol real ending   = Pi_value * 6;
const  real H        = 0.005r;
const  real A        = 5.0r;
real C        = 2.0r / 3.0r;

// --- function ---

function(t) 
{ 
    ^ new Point(
        (A * (C * t.cos() + (C*t).cos()) * ScaleX).RoundedInt,
        (A * (C * t.sin() - (C*t).sin()) * ScaleY).RoundedInt)
}

singleton Plotter
{
    drawAxis(Dimension size, Canvas canvas)
    {
        var zeroX :=  size.Width / 2;
        var zeroY := size.Height / 2 - 10;

        // X axis
        canvas.setCaret(0, zeroY);
        canvas.lineCaretTo(size.Width, zeroY);
        
        canvas.writeText("X", size.Width - 20, zeroY + 2);

        // Y axis
        canvas.setCaret(zeroX, 0);
        canvas.lineCaretTo(zeroX, size.Height);

        canvas.writeText("Y", zeroX + 2, 2);

        for(int i := -10, i <= 10, i += 2)
        {
            if(i != 0)
            {
                canvas.writeText(i.toString(), zeroX + (i * ScaleX) - 4, zeroY + 2);
                
                canvas.writeText(i.toString(), zeroX + 8, zeroY - (i * ScaleY) - 2);
            }
            else
            { 
                canvas.writeText("0", zeroX + 2, zeroY + 2) 
            }
        }
    }
    
    drawGraph(Dimension size, Canvas canvas)
    {
        auto t := starting;
        auto t2 := ending;
        var zero := new Point(size.Width / 2, size.Height / 2 - 10);

        canvas.setCaret(zero + function(t));
        while (t <= t2)
        {
            canvas.lineCaretTo(zero + function(t));

            t := t + H
        }
    }
}

public class MainWindow : SDIDialog
{
    Imagebox theImagebox;

    constructor new()
        <= new()
    {
        theImagebox := Imagebox.new();
        self.appendControl:theImagebox;

        self.Caption := "Graph sample";
        self.setRegion(50, 50, 400, 420);

        theImagebox.setRegion(0, 0, 400, 420);
        theImagebox.onPaint := (object sender, Canvas canvas)
        {
            canvas.set(WhitePen);
            canvas.set(BlackBrush);

            Plotter.drawAxis(theImagebox.Dimension, canvas);
            Plotter.drawGraph(theImagebox.Dimension, canvas)
        }
    }
}