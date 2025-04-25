import { oklch2hex } from 'colorizr'

function oklchStrToHex(oklchStr) {
  const parts = oklchStr.split(' ')

  const lightness = parseFloat(parts[0].replace('%', '')) / 100

  const chroma = parseFloat(parts[1])
  const hue = parseFloat(parts[2])

  let hexColor = oklch2hex({ l: lightness, c: chroma, h: hue })

  return hexColor
}
export function getDaisyUIColor(colorName) {
  const res = getComputedStyle(document.documentElement).getPropertyValue(`--${colorName}`).trim()
  return oklchStrToHex(res)
}
