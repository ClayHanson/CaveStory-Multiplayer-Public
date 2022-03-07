#include "stdafx.h"
#include "Graphics.h"

// This code was written by Vladislav Gelfer of TheCodeProject.com
// and was found in this article http://www.codeproject.com/bitmap/DFB_vs_DIB.asp


// This function converts the given bitmap to a DFB.
// Returns true if the conversion took place,
// false if the conversion either unneeded or unavailable
bool ConvertToDFB(HBITMAP& hBitmap)
{
  bool bConverted = false;
  BITMAP stBitmap;
  if (GetObject(hBitmap, sizeof(stBitmap), &stBitmap) && stBitmap.bmBits)
  {
    // that is a DIB. Now we attempt to create
    // a DFB with the same sizes, and with the pixel
    // format of the display (to omit conversions
    // every time we draw it).
    HDC hScreen = GetDC(NULL);
    if (hScreen)
    {
      HBITMAP hDfb = 
              CreateCompatibleBitmap(hScreen, 
      stBitmap.bmWidth, stBitmap.bmHeight);
      if (hDfb)
      {
        // now let's ensure what we've created is a DIB.
        if (GetObject(hDfb, sizeof(stBitmap), 
                   &stBitmap) && !stBitmap.bmBits)
        {
          // ok, we're lucky. Now we have
          // to transfer the image to the DFB.
          HDC hMemSrc = CreateCompatibleDC(NULL);
          if (hMemSrc)
          {
            HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
            if (hOldSrc)
            {
              HDC hMemDst = CreateCompatibleDC(NULL);
              if (hMemDst)
              {
                HGDIOBJ hOldDst = SelectObject(hMemDst, hDfb);
                if (hOldDst)
                {
                  // transfer the image using BitBlt
                  // function. It will probably end in the
                  // call to driver's DrvCopyBits function.
                  if (BitBlt(hMemDst, 0, 0, 
                        stBitmap.bmWidth, stBitmap.bmHeight, 
                        hMemSrc, 0, 0, SRCCOPY))
                    bConverted = true; // success

                  VERIFY(SelectObject(hMemDst, hOldDst));
                }
                VERIFY(DeleteDC(hMemDst));
              }
              VERIFY(SelectObject(hMemSrc, hOldSrc));
            }
            VERIFY(DeleteDC(hMemSrc));
          }
        }

        if (bConverted)
        {
          VERIFY(DeleteObject(hBitmap)); // it's no longer needed
          hBitmap = hDfb;
        }
        else
          VERIFY(DeleteObject(hDfb));
      }
      ReleaseDC(NULL, hScreen);
    }
  }
  return bConverted;
}

// This function converts the given bitmap to a DIB.
// Returns true if the conversion took place,
// false if the conversion either unneeded or unavailable
bool ConvertToDIB(HBITMAP& hBitmap)
{
  bool bConverted = false;
  BITMAP stBitmap;
  if (GetObject(hBitmap, sizeof(stBitmap), 
            &stBitmap) && !stBitmap.bmBits)
  {
    // that is a DFB. Now we attempt to create
    // a DIB with the same sizes and pixel format.
    HDC hScreen = GetDC(NULL);
    if (hScreen)
    {
      union {
        BITMAPINFO stBitmapInfo;
        BYTE pReserveSpace[sizeof(BITMAPINFO) 
                     + 0xFF * sizeof(RGBQUAD)];
      };
      ZeroMemory(pReserveSpace, sizeof(pReserveSpace));
      stBitmapInfo.bmiHeader.biSize = sizeof(stBitmapInfo.bmiHeader);
      stBitmapInfo.bmiHeader.biWidth = stBitmap.bmWidth;
      stBitmapInfo.bmiHeader.biHeight = stBitmap.bmHeight;
      stBitmapInfo.bmiHeader.biPlanes = 1;
      stBitmapInfo.bmiHeader.biBitCount = stBitmap.bmBitsPixel;
      stBitmapInfo.bmiHeader.biCompression = BI_RGB;

      if (stBitmap.bmBitsPixel <= 8)
      {
        stBitmapInfo.bmiHeader.biClrUsed = 
                        1 << stBitmap.bmBitsPixel;
        // This image is paletted-managed.
        // Hence we have to synthesize its palette.
      }
      stBitmapInfo.bmiHeader.biClrImportant = 
                       stBitmapInfo.bmiHeader.biClrUsed;

      PVOID pBits;
      HBITMAP hDib = CreateDIBSection(hScreen, 
        &stBitmapInfo, DIB_RGB_COLORS, &pBits, NULL, 0);

      if (hDib)
      {
        // ok, we're lucky. Now we have
        // to transfer the image to the DFB.
        HDC hMemSrc = CreateCompatibleDC(NULL);
        if (hMemSrc)
        {
          HGDIOBJ hOldSrc = SelectObject(hMemSrc, hBitmap);
          if (hOldSrc)
          {
            HDC hMemDst = CreateCompatibleDC(NULL);
            if (hMemDst)
            {
              HGDIOBJ hOldDst = SelectObject(hMemDst, hDib);
              if (hOldDst)
              {
                if (stBitmap.bmBitsPixel <= 8)
                {
                  // take the DFB's palette and set it to our DIB
                  HPALETTE hPalette = 
                    (HPALETTE) GetCurrentObject(hMemSrc, OBJ_PAL);
                  if (hPalette)
                  {
                    PALETTEENTRY pPaletteEntries[0x100];
                    UINT nEntries = GetPaletteEntries(hPalette, 
                                    0, stBitmapInfo.bmiHeader.biClrUsed, 
                                    pPaletteEntries);
                    if (nEntries)
                    {
                      ASSERT(nEntries <= 0x100);
                      for (UINT nIndex = 0; nIndex < nEntries; nIndex++)
                        pPaletteEntries[nEntries].peFlags = 0;
                      VERIFY(SetDIBColorTable(hMemDst, 0, 
                        nEntries, (RGBQUAD*) pPaletteEntries) == nEntries);

                    }
                  }
                }

                // transfer the image using BitBlt function.
                // It will probably end in the
                // call to driver's DrvCopyBits function.
                if (BitBlt(hMemDst, 0, 0, stBitmap.bmWidth, 
                      stBitmap.bmHeight, hMemSrc, 0, 0, SRCCOPY))
                  bConverted = true; // success

                VERIFY(SelectObject(hMemDst, hOldDst));
              }
              VERIFY(DeleteDC(hMemDst));
            }
            VERIFY(SelectObject(hMemSrc, hOldSrc));
          }
          VERIFY(DeleteDC(hMemSrc));
        }

        if (bConverted)
        {
          VERIFY(DeleteObject(hBitmap)); // it's no longer needed
          hBitmap = hDib;
        }
        else
          VERIFY(DeleteObject(hDib));
      }
      ReleaseDC(NULL, hScreen);
    }
  }
  return bConverted;
}