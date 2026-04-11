package menu

import (
	"image/color"
	"log"
	"map-editor/src/engine"
	projhandling "map-editor/src/proj-handling"
	"os"
	"path/filepath"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/canvas"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

func HandleCreateProjClick(window fyne.Window) {

	projNameInp := widget.NewEntry()
	projNameInp.SetPlaceHolder("Project name...")

	projDiaglog := dialog.NewForm("Create new project", "Create", "Cancel", []*widget.FormItem{{Text: "Project name", Widget: projNameInp}},
		func(ok bool) {
			if !ok {
				return
			}

			projectName := projNameInp.Text
			log.Println("Creating project: ", projectName)

			projhandling.CreateEmptyProject(projectName)
		}, window)

	projDiaglog.Resize(fyne.NewSize(400, 200))
	projDiaglog.Show()
}
func FileExists(path string) bool {
	_, err := os.Stat(path)
	if err == nil {
		return true
	}
	if os.IsNotExist(err) {
		return false
	}
	return false
}
func HandleOpenProjClick(window fyne.Window, imgMenu *fyne.Container, m *engine.MapViewport) {
	fileDialog := dialog.NewFolderOpen(func(list fyne.ListableURI, err error) {

		if err != nil {
			dialog.ShowError(err, window)
			return
		}
		if list == nil {
			return
		}

		folderPath := list.Path()
		log.Println("Selected dir: ", folderPath)

		if !FileExists(filepath.Join(folderPath, "project.mapGo")) {
			dialog.ShowInformation("Folder is NOT a map project", "This folder does not consist of a project created by this application", window)
			return
		}

		projhandling.LoadProject(window, folderPath, imgMenu, m)

	}, window)
	fileDialog.Show()
}
func HandleSaveProjClick(window fyne.Window, m *engine.MapViewport) {
	projhandling.SaveProject(window, m)
}

func HandleFileClick(button fyne.CanvasObject, window fyne.Window, imgMenu *fyne.Container, m *engine.MapViewport) {
	log.Println("File click")

	createNewProjBtn := fyne.NewMenuItem("Create project", func() { HandleCreateProjClick(window) })
	openExistingProjBtn := fyne.NewMenuItem("Open Project", func() { HandleOpenProjClick(window, imgMenu, m) })
	saveProjBtn := fyne.NewMenuItem("Save Project", func() { HandleSaveProjClick(window, m) })

	menu := fyne.NewMenu("File", createNewProjBtn, openExistingProjBtn, fyne.NewMenuItemSeparator(), saveProjBtn)

	canvas := fyne.CurrentApp().Driver().CanvasForObject(button)
	pos := fyne.CurrentApp().Driver().AbsolutePositionForObject(button)

	btnSize := button.Size()
	menuPos := fyne.NewPos(pos.X, pos.Y+btnSize.Height)

	widget.ShowPopUpMenuAtPosition(menu, canvas, menuPos)
}

func HandleEditClick() {
	log.Println("Doc Click")
}

func CreateToolBar(window fyne.Window, imgMenu *fyne.Container, m *engine.MapViewport) *fyne.Container {

	whiteSeparator := canvas.NewRectangle(color.NRGBA{255, 255, 255, 255})
	whiteSeparator.SetMinSize(fyne.NewSize(2, 2))

	var fileBtn *widget.Button
	fileBtn = widget.NewButtonWithIcon("File", theme.FileIcon(), func() { HandleFileClick(fileBtn, window, imgMenu, m) })
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
