package main

import (
	"bytes"
	b64 "encoding/base64"
	"io"
	"log"
	"net/http"

	"os/exec"

	"github.com/gin-gonic/gin"
)

func upload(c *gin.Context) {
  f, _ := c.FormFile("file")
  file, _ := f.Open() 
  defer file.Close()

  data, _ := io.ReadAll(file)
  encoded := b64.StdEncoding.EncodeToString(data)
  
  cmd := exec.Command("./bin/python3", "image_extractor.py")
  cmd.Dir = "../python/"

  stdin, _ := cmd.StdinPipe()

  cmd2 := exec.Command("./main")
  cmd2.Dir = "../cpp/dlx/drivers/"
  
  var buffer bytes.Buffer
  cmd.Stdout = &buffer

  if err := cmd.Start(); err != nil {
    log.Fatal(err)
  }

  io.WriteString(stdin, encoded)
  io.WriteString(stdin, "\n")
  cmd.Wait()

  stdin, _ = cmd2.StdinPipe()
  var buffer2 bytes.Buffer
  cmd2.Stdout = &buffer2

  if err := cmd2.Start(); err != nil {
    log.Fatal(err)
  }
  io.WriteString(stdin, buffer.String())
  cmd2.Wait()

  c.String(http.StatusOK, buffer2.String())
}

func main() {
  router := gin.Default()

  router.Static("/", "public")
  router.POST("/upload", upload)

  router.Run(":8000")
}

