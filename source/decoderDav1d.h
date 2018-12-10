/*  This file is part of YUView - The YUV player with advanced analytics toolset
*   <https://github.com/IENT/YUView>
*   Copyright (C) 2015  Institut f�r Nachrichtentechnik, RWTH Aachen University, GERMANY
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 3 of the License, or
*   (at your option) any later version.
*
*   In addition, as a special exception, the copyright holders give
*   permission to link the code of portions of this program with the
*   OpenSSL library under certain conditions as described in each
*   individual source file, and distribute linked combinations including
*   the two.
*   
*   You must obey the GNU General Public License in all respects for all
*   of the code used other than OpenSSL. If you modify file(s) with this
*   exception, you may extend this exception to your version of the
*   file(s), but you are not obligated to do so. If you do not wish to do
*   so, delete this exception statement from your version. If you delete
*   this exception statement from all source files in the program, then
*   also delete it here.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DECODERDAV1D_H
#define DECODERDAV1D_H

#include "dav1d.h"
#include "decoderBase.h"
#include "videoHandlerYUV.h"
#include <QLibrary>

struct decoderDav1d_Functions
{
  decoderDav1d_Functions();

  const char *(*dav1d_version)               ();
  void        (*dav1d_default_settings)      (Dav1dSettings*);
  int         (*dav1d_open)                  (Dav1dContext**, const Dav1dSettings*);
  int         (*dav1d_parse_sequence_header) (Dav1dSequenceHeader*, const uint8_t*, const size_t);
  int         (*dav1d_send_data)             (Dav1dContext*, Dav1dData*);
  int         (*dav1d_get_picture)           (Dav1dContext*, Dav1dPicture*);
  void        (*dav1d_close)                 (Dav1dContext**);
  void        (*dav1d_flush)                 (Dav1dContext*);

  uint8_t    *(*dav1d_data_create)           (Dav1dData *data, size_t sz);
};

// This class wraps the libde265 library in a demand-load fashion.
class decoderDav1d : public decoderBaseSingleLib, public decoderDav1d_Functions 
{
public:
  decoderDav1d(int signalID, bool cachingDecoder=false);
  ~decoderDav1d();

  void resetDecoder() Q_DECL_OVERRIDE;

  int nrSignalsSupported() const Q_DECL_OVERRIDE { return nrSignals; }
  bool isSignalDifference(int signalID) const Q_DECL_OVERRIDE { return signalID == 2 || signalID == 3; }
  QStringList getSignalNames() const Q_DECL_OVERRIDE { return QStringList() << "Reconstruction" << "Prediction" << "Residual" << "Transform Coefficients"; }
  void setDecodeSignal(int signalID, bool &decoderResetNeeded) Q_DECL_OVERRIDE;

  // Decoding / pushing data
  bool decodeNextFrame() Q_DECL_OVERRIDE;
  QByteArray getRawFrameData() Q_DECL_OVERRIDE;
  bool pushData(QByteArray &data) Q_DECL_OVERRIDE;

  // Check if the given library file is an existing libde265 decoder that we can use.
  static bool checkLibraryFile(QString libFilePath, QString &error);

  QString getDecoderName() const Q_DECL_OVERRIDE;
  QString getCodecName()         Q_DECL_OVERRIDE { return "AV1"; }

private:
  // A private constructor that creates an uninitialized decoder library.
  // Used by checkLibraryFile to check if a file can be used as a hevcDecoderLibde265.
  decoderDav1d() : decoderBaseSingleLib() {};

  // Try to resolve all the required function pointers from the library
  void resolveLibraryFunctionPointers() Q_DECL_OVERRIDE;

  // Return the possible names of the dav1d library
  QStringList getLibraryNames() Q_DECL_OVERRIDE;

  // The function template for resolving the functions.
  // This can not go into the base class because then the template
  // generation does not work.
  template <typename T> T resolve(T &ptr, const char *symbol, bool optional=false);

  void allocateNewDecoder();

  Dav1dContext *decoder {nullptr};
  Dav1dSettings settings;

  int nrSignals {1};
  bool flushing {false};
  bool sequenceHeaderPushed {false};

  // When pushing frames, the decoder will try to decode a frame to check if this is possible.
  // If this is true, a frame is waiting from that step and decodeNextFrame will not actually decode a new frame.
  bool decodedFrameWaiting {false};

  // Try to decode a frame. If successfull, the frame will be pointed to by curImage.
  bool decodeFrame();
  const Dav1dPicture* curImage {nullptr};

  // With the given partitioning mode, the size of the CU and the prediction block index, calculate the
  // sub-position and size of the prediction block
  void getPBSubPosition(int partMode, int CUSizePix, int pbIdx, int *pbX, int *pbY, int *pbW, int *pbH) const;
  void cacheStatistics_TUTree_recursive(uint8_t *const tuInfo, int tuInfoWidth, int tuUnitSizePix, int iPOC, int tuIdx, int tuWidth_units, int trDepth, bool isIntra, uint8_t *const intraDirY, uint8_t *const intraDirC, int intraDir_infoUnit_size, int widthInIntraDirUnits);

  // We buffer the current image as a QByteArray so you can call getYUVFrameData as often as necessary
  // without invoking the copy operation from the libde265 buffer to the QByteArray again.
#if SSE_CONVERSION
  byteArrayAligned currentOutputBuffer;
  void copyImgToByteArray(const de265_image *src, byteArrayAligned &dst);
#else
  QByteArray currentOutputBuffer;
  void copyImgToByteArray(const Dav1dPicture *src, QByteArray &dst);   // Copy the raw data from the Dav1dPicture source *src to the byte array
#endif
};

#endif // DECODERDAV1D_H
