package main

import (
	"image/color"
	"map-editor/src/engine"

	"fyne.io/fyne/v2/app"
)

func main() {
	mapEditor := app.New()
	window := mapEditor.NewWindow("Map Editor")

	gameMap := engine.NewMapViewport()
	gameMap.MapData[engine.TileCoord{X: 0, Y: 0}] = engine.Tile{Color: color.RGBA{255, 0, 0, 255}}

	window.SetContent(gameMap)
	window.ShowAndRun()
}
