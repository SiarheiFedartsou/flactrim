#ifndef TRIMMER_HPP
#define TRIMMER_HPP

#include "BitOStream.hpp"
#include "BitIStream.hpp"
#include "flac_defines.hpp"
#include "exceptions.hpp"



using namespace std;

class Trimmer
{
	public:
		Trimmer(std::string FLACFileName) : fileName(FLACFileName)
		{

		}

		void CutTrack(string outputFLACFile, unsigned int leftSecond, unsigned int rightSecond);
		~Trimmer()
		{
		}
	private:
		string fileName;

		void ReadMetaBlockHeader(BitIStream& bs, FLACMetaBlockHeader * mbh);
		void WriteMetaBlockHeader(BitOStream& bs, FLACMetaBlockHeader * mbh);

		void ReadStreamInfo(BitIStream& bs, FLACMetaStreamInfo * msi);
		void WriteStreamInfo(BitOStream& bs, FLACMetaStreamInfo * msi);

		void ReadFrameHeader(BitIStream& bs, FLACFrameHeader * fh);
		void WriteFrameHeader(BitOStream& bs, FLACFrameHeader * fh);


		void ReadSubframeHeader(BitIStream& bs, FLACSubframeHeader * sfh);
		void WriteSubframeHeader(BitOStream& bs, FLACSubframeHeader * sfh);

		void CopyConstantSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi);
		void CopyVerbatimSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi);
		void CopyFixedSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);
		void CopyLPCSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);
		void CopyResidual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);
		void CopyRiceResidual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);
		void CopyRice2Residual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);




		void CopyBytes(BitIStream& bis, BitOStream& bos, size_t n);
		void CopyBits(BitIStream& bis, BitOStream& bos, size_t n);
		
		uint8_t GetPredictorOrder(FLACSubframeHeader * sfh);
		uint32_t GetBlockSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi);
		uint8_t GetBitsPerSample(FLACFrameHeader * fh, FLACMetaStreamInfo * msi);
		uint16_t GetUnencSubblockBitSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi);
		uint16_t GetWarmUpSamplesBitSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh);
	size_t c;
		/*	BitIStream * bis;
		BitOStream * bos;*/
};

#endif
