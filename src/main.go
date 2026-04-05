package main

import (
	"image/color"
	toolbar "map-editor/src/app"
	"map-editor/src/engine"

	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"
)

func main() {
	mapEditor := app.New()
	window := mapEditor.NewWindow("Map Editor")

	toolbar := toolbar.CreateToolBar()

	gameMap := engine.NewMapViewport()
	gameMap.MapData[engine.TileCoord{X: 0, Y: 0}] = engine.Tile{Color: color.RGBA{255, 0, 0, 255}}

	content := container.NewBorder(toolbar, nil, nil, nil, gameMap)
	window.SetContent(content)
	window.ShowAndRun()
}
