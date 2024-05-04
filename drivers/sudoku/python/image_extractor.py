from imutils.perspective import four_point_transform
from skimage.segmentation import clear_border
import sys
import base64
import pytesseract
import numpy as np
import imutils
import cv2

def puzzle_from_image(image):
    grey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    blurred = cv2.GaussianBlur(grey, (7,7), 3)
    threshold = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY, 11, 2)
    threshold = cv2.bitwise_not(threshold)

    contours = cv2.findContours(threshold.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)
    contours = sorted(contours, key=cv2.contourArea, reverse=True)

    puzzle_contour = None

    for contour in contours:
        arc_length = cv2.arcLength(contour, True)
        approximation = cv2.approxPolyDP(contour, 0.02 * arc_length, True)

        if len(approximation) == 4:
            puzzle_contour = approximation
            break

    output = image.copy()
    cv2.drawContours(output, [puzzle_contour], -1, (0, 0, 255), 3)
    warped = four_point_transform(grey, puzzle_contour.reshape(4,2))

    return warped

def digits_from_puzzle(puzzle):
    step_x = puzzle.shape[1] // 9
    step_y = puzzle.shape[0] // 9
    cells = []

    for y in range(9):
        rows = []
        for x in range(9):
            start_x = x * step_x
            start_y = y * step_y
            end_x = (x + 1) * step_x
            end_y = (y + 1) * step_y

            digit = process_digit(puzzle[start_y:end_y, start_x:end_x])
            if digit is None:
                rows.append(0)
                continue
            s = pytesseract.image_to_string(digit, config="--psm 10 --oem 3 -c tessedit_char_whitelist=0123456789")
            x = int(s) if s != "" else 0
            rows.append(x)
        cells.append(rows)
    return cells

def process_digit(cell):
    threshold = cv2.threshold(cell, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]
    threshold = clear_border(threshold)
    
    contours = cv2.findContours(threshold.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    contours = imutils.grab_contours(contours)

    if len(contours) == 0:
        return None

    contour = max(contours, key = cv2.contourArea)
    mask = np.zeros(threshold.shape, dtype = "uint8")
    cv2.drawContours(mask, [contour], -1, 255, -1)

    (h,w) = threshold.shape
    percent_size = cv2.countNonZero(mask) / float(w * h)

    if percent_size < 0.03:
        return None

    digit = cv2.bitwise_and(threshold, threshold, mask=mask)
    return digit

def serialize_digits(digits):
    ret = ""
    for row in digits:
        for num in row:
            ret += str(num)
    return ret + "\n"


def process_image(image_b64):
    image = np.frombuffer(base64.b64decode(image_b64), np.uint8)
    puzzle = puzzle_from_image(cv2.imdecode(image, cv2.IMREAD_COLOR))
    digits = digits_from_puzzle(puzzle)
    return serialize_digits(digits)

if __name__ == "__main__":
    print(process_image(input()))
