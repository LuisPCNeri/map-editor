package toolbar

import (
	"image/color"
	"log"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

func HandleFileClick() {
	log.Println("File click")
}
func HandleEditClick() {
	log.Println("Doc Click")
}

func CreateToolBar() *fyne.Container {

	whiteSeparator := canvas.NewRectangle(color.NRGBA{255, 255, 255, 255})
	whiteSeparator.SetMinSize(fyne.NewSize(2, 2))

	fileBtn := widget.NewButtonWithIcon("File", theme.FileIcon(), HandleFileClick)
	fileBtn.Importance = widget.LowImportance

	editBtn := widget.NewButton("Edit", HandleEditClick)
	editBtn.Importance = widget.LowImportance

	toolbar := container.NewHBox(
		fileBtn,
		container.NewPadded(whiteSeparator),

		editBtn,
	)

	return toolbar
}
