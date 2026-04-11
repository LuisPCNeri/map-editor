package menu

import (
	"log"
	"map-editor/src/engine"
	projhandling "map-editor/src/proj-handling"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/container"
	"fyne.io/fyne/v2/dialog"
	"fyne.io/fyne/v2/storage"
	"fyne.io/fyne/v2/theme"
	"fyne.io/fyne/v2/widget"
)

func HandleAddImg(window fyne.Window, imgMenu *fyne.Container, m *engine.MapViewport) {

	fileDialog := dialog.NewFileOpen(func(reader fyne.URIReadCloser, err error) {

		if err != nil {
			dialog.ShowError(err, window)
			return
		}
		if reader == nil {
			return
		}
		defer reader.Close()

		fileURI := reader.URI()
		log.Println("Selected img: ", fileURI.Path())

		res := m.LoadResource(fileURI)
		imgBtn := engine.NewTappableImage(res, func() {
			for coord := range m.SelectedTiles {
				tile := m.MapData[coord]
				tile.ImgURI = fileURI
				tile.Resource = res
				tile.Coords = coord
				m.MapData[coord] = tile
			}
			m.Refresh()
		})
		imgMenu.Add(imgBtn)
		imgMenu.Refresh()

		projhandling.AddImgToAssetsFolder(fileURI.Path())

	}, window)
	fileDialog.SetFilter(storage.NewExtensionFileFilter([]string{".png", ".jph", ".jpeg"}))
	fileDialog.Show()
}

func CreateImageInputRect(window fyne.Window, m *engine.MapViewport) (*fyne.Container, *fyne.Container) {

	imgMenu := container.NewHBox()
	addBtn := widget.NewButtonWithIcon("", theme.ContentAddIcon(), func() { HandleAddImg(window, imgMenu, m) })

	btnsRow := container.NewHBox(
		addBtn,
	)

	menu := container.NewVBox(
		btnsRow,
		imgMenu,
	)

	return menu, imgMenu
}
