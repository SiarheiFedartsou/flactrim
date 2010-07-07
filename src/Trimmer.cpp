#include "Trimmer.hpp"

void Trimmer::CutTrack(string outputFLACFile, unsigned int leftSecond, unsigned int rightSecond)
{
	uint8_t * buf;
	BitIStream bis(fileName);
	BitOStream bos(outputFileName);
	FLACMetaStreamInfo msi;

	if (bis.ReadString(4) != "fLaC")
	{
		throw NoFLACFile();
	}
	bos.WriteString("fLaC");
			
	FLACMetaBlockHeader mbh;
	mbh.IsLastBlock = false;
			
	while (!mbh.IsLastBlock)
	{
		ReadMetaBlockHeader(bis, &mbh);
		WriteMetaBlockHeader(bos, &mbh);
		switch (mbh.BlockType)
		{
			case FLAC_META_STREAMINFO:
				ReadStreamInfo(bis, &msi);
				WriteStreamInfo(bos, &msi)
			break;

			case FLAC_META_SEEKTABLE:
			case FLAC_META_APPLICATION:
			case FLAC_META_VORBIS_COMMENT:
			case FLAC_META_CUESHEET:
			case FLAC_META_PADDING:
				CopyBytes(bis, bos, mbh.BlockSize);
			break;

			default:
				cerr << "Warning: unknown block type" << endl;
				CopyBytes(bis, bos, mbh.BlockSize);
			break;
		}
	}
	FLACFrameHeader fh;
	ReadFrameHeader(bis, &fh);
	WriteFrameHeader(bos, &fh);
	
	for (int subframeN = 0; subframeN <= channelsCount; subframeN++)
	{
		FLACSubframeHeader sfh;
		ReadSubframeHeader(bis, &sfh);
		WriteSubframeHeader(bos, &sfh);
				
		switch (sfh.type)
		{
			case FLAC_SF_CONSTANT:
				CopyConstantSubframe(bis, bos, GetBitsPerSample(fh, msi));
			break;
			case FLAC_SF_VERBATIM:
			break;
			case FLAC_SF_FIXED:
			break;
			case FLAC_SF_LPC:
			break;
			default:
			break;
		}
	}
}

void Trimmer::ReadMetaBlockHeader(BitIStream& bs, FLACMetaBlockHeader * mbh)
{
	bs.ReadFlag(&mbh->IsLastBlock);
	bs.ReadInteger(&mbh->BlockType, 7);
	bs.ReadInteger(&mbh->BlockSize, 24);
}

void Trimmer::WriteMetaBlockHeader(BitOStream& bs, FLACMetaBlockHeader * mbh)
{
	bs.WriteFlag(mbh->IsLastBlock);
	bs.WriteInteger(mbh->BlockType, 7);
	bs.WriteInteger(mbh->BlockSize, 24);
}

void Trimmer::ReadStreamInfo(BitIStream& bs, FLACMetaStreamInfo * msi)
{
	bs.ReadInteger(&msi->MinBlockSize, 16);
	bs.ReadInteger(&msi->MaxBlockSize, 16);
	bs.ReadInteger(&msi->MinFrameSize, 24);
	bs.ReadInteger(&msi->MaxFrameSize, 24);
	bs.ReadInteger(&msi->Sampling, 20);
	bs.ReadInteger(&msi->ChannelsCount, 3);
	bs.ReadInteger(&msi->BitsPerSample, 5);
	bs.ReadInteger(&msi->StreamSamplesCount, 36);
	bs.ReadAlignBuffer(msi->md5, 16);
}

void Trimmer::WriteStreamInfo(BitOStream& bs, FLACMetaStreamInfo * msi)
{
	bs.WriteInteger(msi->MinBlockSize, 16);
	bs.WriteInteger(msi->MaxBlockSize, 16);
	bs.WriteInteger(msi->MinFrameSize, 24);
	bs.WriteInteger(msi->MaxFrameSize, 24);
	bs.WriteInteger(msi->Sampling, 20);
	bs.WriteInteger(msi->ChannelsCount, 3);
	bs.WriteInteger(msi->BitsPerSample, 5);
	bs.WriteInteger(msi->StreamSamplesCount, 36);
	bs.WriteAlignBuffer(msi->md5, 16);
}



void Trimmer::ReadFrameHeader(BitIStream& bs, FLACFrameHeader * fh)
{
	uint64_t zero = 0;
	bs.ReadInteger(&fh->SyncCode, 14);
	bs.ReadInteger(&zero, 2);
	bs.ReadInteger(&fh->BlockSize, 4);
	bs.ReadInteger(&fh->Sampling, 4);
	bs.ReadInteger(&fh->ChannelsLocation, 4);
	bs.ReadInteger(&fh->BitsPerSample, 3);
	bs.ReadInteger(&zero, 1);

	switch (fh->BlockSize)
	{
		case 0b0110:
			bs.ReadInteger(&fh->UTF8Num, 36);
			bs.ReadInteger(&fh->VariableBlockSize, 8);
			fh->IsVariableBlockSize = true;
		break;

		case 0b0111:
			bs.ReadInteger(&fh->UTF8Num, 36);
			bs.ReadInteger(&fh->VariableBlockSize, 16);
			fh->IsVariableBlockSize = true;
		break;

		default:
			bs.ReadInteger(&fh->UTF8Num, 31);
			fh->IsVariableBlockSize = false;
		break;
	}

	switch (fh->Sampling)
	{
		case 0b1100:
			bs.ReadInteger(&fh->VariableSampling, 8);
			fh->IsVariableSampling = true;
		break;

		case 0b1101:
		case 0b1110:
			bs.ReadInteger(&fh->VariableSampling, 16);
			fh->IsVariableSampling = true;
		break;

		default:
			fh->IsVariableSampling = false;
		break;
	}
	bs.ReadInteger(&fh->ControlSum, 8);
}

void Trimmer::WriteFrameHeader(BitOStream& bs, FLACFrameHeader * fh)
{
	bs.WriteInteger(fh->SyncCode, 14);
	bs.WriteInteger(0, 2);
	bs.WriteInteger(fh->BlockSize, 4);
	bs.WriteInteger(fh->Sampling, 4);
	bs.WriteInteger(fh->ChannelsLocation, 4);
	bs.WriteInteger(fh->BitsPerSample, 3);
	bs.WriteInteger(0, 1);

	switch (fh->BlockSize)
	{
		case 0b0110:
			bs.WriteInteger(fh->UTF8Num, 36);
			bs.WriteInteger(fh->VariableBlockSize, 8);
		break;

		case 0b0111:
			bs.WriteInteger(fh->UTF8Num, 36);
			bs.WriteInteger(fh->VariableBlockSize, 16);
		break;

		default:
			bs.WriteInteger(fh->UTF8Num, 31);
		break;
	}

	switch (fh->Sampling)
	{
		case 0b1100:
			bs.WriteInteger(fh->VariableSampling, 8);
		break;

		case 0b1101:
		case 0b1110:
			bs.WriteInteger(fh->VariableSampling, 16);
		break;
	}	
	bs.WriteInteger(fh->ControlSum, 8);
}



void Trimmer::ReadSubframeHeader(BitIStream& bs, FLACSubframeHeader * sfh)
{
	uint8_t zero = 0;
	uint8_t sf_type = 0;
	uint64_t waste = 0;
	uint8_t bit = 0;
	bool wastedBitsFlag = false;

	bs.ReadInteger(&zero, 1);
	bs.ReadInteger(&sf_type, 6);
	if (sf_type == 0) { //constant
		sfh->type = FLAC_SF_CONSTANT;
	} else if (sf_type == 1) { //verbatim
		sfh->type = FLAC_SF_VERBATIM;
	} else if (sf_type & 0b100000) { //lpc
		sfh->order = (sf_type & 0b011111) + 1;
		sfh->type = FLAC_SF_LPC;
	} else if (sf_type & 0b001000) { //fixed
		sfh->order = sf_type & 0b000111;
		sfh->type = FLAC_SF_FIXED;
	}
	bs.ReadFlag(&wastedBitsFlag);

	if (wastedBitsFlag)
	{
		while (bit == 0)
		{
			waste++;
			bs.ReadInteger(&bit, 1);
		}
		sfh->waste = waste;
	}
	else 
	{
		sfh->waste = 0;
	}
}

void Trimmer::WriteSubframeHeader(BitOStream& bs, FLACSubframeHeader * sfh)
{
	uint8_t type = 0;
	uint8_t one = 1;
	bs.WriteInteger(0, 1);
	switch (sfh->type)
	{
		case FLAC_SF_CONSTANT:
			type = 0;
			bs.WriteInteger(type, 6);
		break;

		case FLAC_SF_VERBATIM:
			type = 1;
			bs.WriteInteger(type, 6);
		break;

		case FLAC_SF_FIXED:
			type = 0b001000 | (uint8_t)sfh->order;
			bs.WriteInteger(type, 6);
		break;

		case FLAC_SF_LPC:
			type = 0b100000 | ((uint8_t)sfh->order - 1);
			bs.WriteInteger(type, 6);
		break;
	}
	bs.WriteFlag(sfh->waste != 0);

	if (sfh->waste != 0)
	{
		for (; sfh->waste; sfh->waste--)
		{
			bs.WriteInteger(0, 1);
		}
		bs.WriteInteger(one, 1);
	}
}


void Trimmer::CopyBytes(BitIStream& bis, BitOStream& bos, size_t n)
{
	uint8_t * buf = new uint8_t[n];
	bis.ReadAlignBuffer(buf, n);
	bos.WriteAlignBuffer(buf, n);
	delete[] buf;
}

uint8_t Trimmer::GetBitsPerSample(FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{
	switch (fh->BitsPerSample)
	{
		case 0b001:
			return 8;
		case 0b010:
			return 12;
		case 0b100:
			return 16;
		case 0b101:
			return 20;
		case 0b110:
			return 24;
		default:
			return msi->BitsPerSample + 1;
	}
}

void Trimmer::CopyConstantSubframe(BitIStream& bis, BitOStream& bos, size_t bitsCount)
{
	uint64_t constant = 0;
	bis.ReadInteger(&constant, bitsCount);
	bos.WriteInteger(constant, bitsCount);
}


uint16_t Trimmer::GetUnencSubblockSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{

}

void CopyVerbatimSubframe(BitIStream& bis, BitOStream& bos, size_t bytesCount);
{
	CopyBytes(bis, bos, bytesCount);
}

