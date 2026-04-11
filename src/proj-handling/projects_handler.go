package projhandling

import (
	"encoding/json"
	"io"
	"log"
	"map-editor/src/engine"
	"os"
	"path/filepath"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/driver/desktop"
	"fyne.io/fyne/v2/storage"
	"fyne.io/fyne/v2/widget"
)

func CreateEmptyProject(projName string) {
	home, err := os.UserHomeDir()
	if err != nil {
		log.Println("Error finding home directory:", err)
		return
	}

	projPath := filepath.Join(home, projName)
	assetsPath := filepath.Join(projPath, "assets")

	if err := os.MkdirAll(assetsPath, 0755); err != nil {
		log.Println("Error creating project directories:", err)
		return
	}

	os.WriteFile(filepath.Join(projPath, "project.mapGo"), nil, 0644)

	os.Chdir(projPath)
}

func AddImgToAssetsFolder(imgPath string) {
	fName := filepath.Base(imgPath)
	dstPath := filepath.Join("assets", fName)

	src, err := os.Open(imgPath)
	if err != nil {
		log.Println("Error opening source image:", err)
		return
	}
	defer src.Close()

	dst, err := os.Create(dstPath)
	if err != nil {
		log.Println("Error creating destination image:", err)
		return
	}
	defer dst.Close()

	_, err = io.Copy(dst, src)
	if err != nil {
		log.Println("Error copying file:", err)
	}
}

func SaveProject(m *engine.MapViewport) {
	/// This is assuming we already are on an open project
	/// So the JSON file should be at .../projPath/JSON
	f, err := os.Create("map-data.json")
	if err != nil {
		log.Println("Error creating map-data.json:", err)
		return
	}
	defer f.Close()

	encoder := json.NewEncoder(f)
	for coord, tileData := range m.MapData {
		tileData.Coords = coord
		if err := encoder.Encode(tileData); err != nil {
			log.Println("Error encoding tile:", err)
		}
	}
}

func LoadProject(projPath string, imgMenu *fyne.Container, m *engine.MapViewport) {
	os.Chdir(projPath)

	// Clear existing data to prevent interference from previous projects
	m.MapData = make(map[engine.TileCoord]engine.Tile)
	m.ResCache = make(map[string]fyne.Resource)
	m.SelectedTiles = make(map[engine.TileCoord]bool)
	imgMenu.Objects = nil

	items, _ := os.ReadDir("./assets")
	for _, item := range items {
		if item.IsDir() {
			subItems, _ := os.ReadDir(filepath.Join("assets", item.Name()))
			for _, subItem := range subItems {
				if !subItem.IsDir() {
					fullPath := filepath.Join(projPath, "assets", item.Name(), subItem.Name())
					u := storage.NewFileURI(fullPath)
					res := m.LoadResource(u)

					imgBtn := newTappableImage(res, func() {
						for coord := range m.SelectedTiles {
							tile := m.MapData[coord]
							tile.ImgURI = u
							tile.Coords = coord
							m.MapData[coord] = tile
						}
						m.Refresh()
					})
					imgMenu.Add(imgBtn)
				}
			}
		} else {
			fullPath := filepath.Join(projPath, "assets", item.Name())
			u := storage.NewFileURI(fullPath)
			res := m.LoadResource(u)

			imgBtn := newTappableImage(res, func() {
				for coord := range m.SelectedTiles {
					tile := m.MapData[coord]
					tile.ImgURI = u
					tile.Coords = coord
					m.MapData[coord] = tile
				}
				m.Refresh()
			})
			imgMenu.Add(imgBtn)
		}
	}

	imgMenu.Refresh()

	jsonFile := "./map-data.json"
	f, err := os.Open(jsonFile)
	if err != nil {
		if !os.IsNotExist(err) {
			log.Println("Error opening map-data.json:", err)
		}
		return
	}
	defer f.Close()

	decoder := json.NewDecoder(f)
	for {
		var tile engine.Tile
		if err := decoder.Decode(&tile); err == io.EOF {
			break
		} else if err != nil {
			log.Println("Error decoding tile:", err)
			break
		}
		m.MapData[tile.Coords] = tile
		if tile.ImgURI != nil {
			m.LoadResource(tile.ImgURI)
			m.Refresh()
		}
	}
}

type tappableImage struct {
	widget.Icon
	onTap func()
}

func newTappableImage(res fyne.Resource, onTap func()) *tappableImage {
	t := &tappableImage{onTap: onTap}
	t.ExtendBaseWidget(t)
	t.SetResource(res)
	return t
}

func (t *tappableImage) Tapped(_ *fyne.PointEvent) {
	if t.onTap != nil {
		t.onTap()
	}
}

func (t *tappableImage) Cursor() desktop.Cursor {
	return desktop.PointerCursor
}

func (t *tappableImage) MinSize() fyne.Size {
	return fyne.NewSize(64, 64)
}
