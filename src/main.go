package main

import (
	"image/color"
	menu "map-editor/src/app"
	"map-editor/src/engine"

	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
)

func main() {
	mapEditor := app.New()
	window := mapEditor.NewWindow("Map Editor")
	window.CenterOnScreen()

	gameMap := engine.NewMapViewport()
	gameMap.MapData[engine.TileCoord{X: 0, Y: 0}] = engine.Tile{Color: color.RGBA{255, 0, 0, 255}, Coords: engine.TileCoord{X: 0, Y: 0}}

	imgMenuWrapper, imgList := menu.CreateImageInputRect(window, gameMap)
	toolbar := menu.CreateToolBar(window, imgList, gameMap)

	content := container.NewBorder(toolbar, imgMenuWrapper, nil, nil, gameMap)
	window.SetContent(content)
	window.ShowAndRun()
}
