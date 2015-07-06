#define system.

// --- control types ---
#subject(class:forms'IControl)          control.

#subject(class:forms'Frame)             frame.
#subject(class:forms'StaticLabel)       static.
#subject(class:forms'Label)             label.
#subject(class:forms'Edit)              edit.
#subject(class:forms'Memo)              memo.
#subject(class:forms'Button)            button.
#subject(class:forms'RadioButton)       radiobutton.
#subject(class:forms'Paintbox)          paintbox.
#subject(class:forms'Panel)             panel.
#subject(class:forms'RadioButtonGroup)  radiogroup.
#subject(class:forms'ImageList)         imagelist.
#subject(class:forms'SDIForm)           sdiform.
#subject(class:forms'ChildForm)         form.

// --- properties ---
#subject caption.
#subject visible.
#subject enabled.
#subject checked.
#subject parent.
#subject tabstop.
#subject font.
#subject back_color.
#subject fore_color.
#subject opened.
#subject items.
#subject menu.
#subject menu_id.
#subject selected_index.

#subject(class:system'drawing'Point)location.

#subject(class:forms'ControlCollection)controls.

// --- event handlers ---
#subject(class:system'BaseFunction1) onClick.
#subject(class:system'BaseFunction1) onIndexChanged.
#subject(class:system'BaseFunction1) onResize.
#subject onPaint.