#include "Trimmer.hpp"

void Trimmer::CutTrack(string outputFLACFile, unsigned int leftSecond, unsigned int rightSecond)
{
   /* if (bs.GetString(3) == "ID3")
    {
    	bs.Skip(3);
    	uint32_t id3size = 0;
    	uint32_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;
    	bs.GetInteger(&b1, 8);
    	bs.GetInteger(&b2, 8);
    	bs.GetInteger(&b3, 8);
    	bs.GetInteger(&b4, 8);
    	id3size = (b1 << 21) | (b2 << 14) | (b3 << 7) | b4;
    	bs.Skip(id3size);
    }*/

	BitIStream bis(fileName);
	BitOStream bos(outputFLACFile);
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
				WriteStreamInfo(bos, &msi);
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
//	for (int i = 0; i < 20; i++)
//	{

		//Frame start
		FLACFrameHeader fh;
		ReadFrameHeader(bis, &fh);
		WriteFrameHeader(bos, &fh);
		for (int subframeN = 0; subframeN <= msi.ChannelsCount; subframeN++)
		{
			FLACSubframeHeader sfh;
			ReadSubframeHeader(bis, &sfh);
			WriteSubframeHeader(bos, &sfh);
			switch (sfh.Type)
			{
				case FLAC_SF_CONSTANT:	
					CopyConstantSubframe(bis, bos, &fh, &msi);
					cout << "const" << endl;
				break;
				case FLAC_SF_VERBATIM:
					CopyVerbatimSubframe(bis, bos, &fh, &msi);
					cout << "verbatim" << endl;
				break;
				case FLAC_SF_FIXED:
					CopyFixedSubframe(bis, bos, &fh, &msi, &sfh);
					cout << "fixed" << endl;
				break;
				case FLAC_SF_LPC:
					CopyLPCSubframe(bis, bos, &fh, &msi, &sfh);
					cout << "lpc" << endl;
				break;
				default:
				break;
			}
		}
		bos.AlignByte();
		uint16_t crc16;
		bis.ReadInteger(&crc16, 16);
		bos.WriteInteger(crc16, 16);

	ReadFrameHeader(bis, &fh);
	cout << fh.SyncCode << endl;
		//Frame end

//	}
		
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
	bs.ReadInteger(&fh->SyncCode, 14);
	bs.ReadInteger(&fh->Zero, 2);
	bs.ReadInteger(&fh->BlockSize, 4);
	bs.ReadInteger(&fh->Sampling, 4);
	bs.ReadInteger(&fh->ChannelsLocation, 4);
	bs.ReadInteger(&fh->BitsPerSample, 3);
	bs.ReadInteger(&fh->Zero2, 1);

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
	bs.WriteInteger(fh->Zero, 2);
	bs.WriteInteger(fh->BlockSize, 4);
	bs.WriteInteger(fh->Sampling, 4);
	bs.WriteInteger(fh->ChannelsLocation, 4);
	bs.WriteInteger(fh->BitsPerSample, 3);
	bs.WriteInteger(fh->Zero2, 1);

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
	uint8_t sf_type = 0;

	bs.ReadInteger(&sfh->Zero, 1);
	cout << (int)sfh->Zero << endl;
	bs.ReadInteger(&sf_type, 6);
	if (sf_type == 0) { //constant
		sfh->Type = FLAC_SF_CONSTANT;
	} else if (sf_type == 1) { //verbatim
		sfh->Type = FLAC_SF_VERBATIM;
	} else if (sf_type & 0b100000) { //lpc
		sfh->Order = (sf_type & 0b011111) + 1;
		sfh->Type = FLAC_SF_LPC;
	} else if (sf_type & 0b001000) { //fixed
		sfh->Order = sf_type & 0b000111;
		sfh->Type = FLAC_SF_FIXED;
	}
	bs.ReadFlag(&sfh->IsWasted);
	if (sfh->IsWasted)
	{
		bs.ReadUnary(&sfh->Waste);
	}
}

void Trimmer::WriteSubframeHeader(BitOStream& bs, FLACSubframeHeader * sfh)
{
	uint8_t type = 0;
	bs.WriteInteger(sfh->Zero, 1);
	switch (sfh->Type)
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
			type = 0b001000 | (uint8_t)sfh->Order;
			bs.WriteInteger(type, 6);
		break;

		case FLAC_SF_LPC:
			type = 0b100000 | ((uint8_t)sfh->Order - 1);
			bs.WriteInteger(type, 6);
		break;
	}
	bs.WriteFlag(sfh->IsWasted);

	if (sfh->IsWasted)
	{
		bs.WriteUnary(sfh->Waste);
	}
}

//TODO: write separated CopyBytes functions for align buffer and no align buffer
void Trimmer::CopyBytes(BitIStream& bis, BitOStream& bos, size_t n)
{
	uint8_t * buf = new uint8_t[n];
	bis.ReadBuffer(buf, n);
	bos.WriteBuffer(buf, n);
	delete[] buf;
}

void Trimmer::CopyBits(BitIStream& bis, BitOStream& bos, size_t n)
{
	c+=n;
	uint64_t tmp = 0;
	while (n / (sizeof(uint64_t) * BITSINBYTE) > 0)
	{
		bis.ReadInteger(&tmp, sizeof(uint64_t) * BITSINBYTE);
		bos.WriteInteger(tmp, sizeof(uint64_t) * BITSINBYTE);
		n -= sizeof(uint64_t) * BITSINBYTE;
	}
	if (n > 0)
	{
		bis.ReadInteger(&tmp, n);
		bos.WriteInteger(tmp, n);
	}

	
}


void Trimmer::CopyConstantSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{
	size_t bitsCount = GetBitsPerSample(fh, msi);
	uint64_t constant = 0;
	bis.ReadInteger(&constant, bitsCount);
	bos.WriteInteger(constant, bitsCount);
}

void Trimmer::CopyVerbatimSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{
	if (GetUnencSubblockBitSize(fh, msi) % BITSINBYTE != 0) cerr << "GetUnencSubblockBitSize(fh, msi) % BITSINBYTE != 0" << endl;
	CopyBytes(bis, bos, GetUnencSubblockBitSize(fh, msi) / BITSINBYTE);
}

void Trimmer::CopyFixedSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	//--------------------------------------------------
	// size_t size 
	// if (GetWarmUpSamplesBitSize(fh, msi, sfh) % BITSINBYTE != 0) cerr << "GetWarmUpSamplesBitSize(fh, msi) % BITSINBYTE != 0" << endl;
	// CopyBytes(bis, bos, GetWarmUpSamplesBitSize(fh, msi, sfh) / BITSINBYTE);
	//-------------------------------------------------- 
	size_t bitSize = GetWarmUpSamplesBitSize(fh, msi, sfh);
	CopyBits(bis, bos, bitSize);
	/*if (bitSize % BITSINBYTE != 0) CopyBits(bis, bos, bitSize);
	else CopyBytes(bis, bos, bitSize / BITSINBYTE);*/
	
	CopyResidual(bis, bos, fh, msi, sfh);

}

void Trimmer::CopyLPCSubframe(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	uint8_t qlp = 0; // quantized linear predictor
	uint8_t qlpCoeffShift = 0;
	uint16_t unencQLPCoeffsBitSize = 0;
	if (GetWarmUpSamplesBitSize(fh, msi, sfh) % BITSINBYTE != 0) cerr << "GetWarmUpSamplesBitSize(fh, msi) % BITSINBYTE != 0" << endl;
	CopyBytes(bis, bos, GetWarmUpSamplesBitSize(fh, msi, sfh) / BITSINBYTE);
	bis.ReadInteger(&qlp, 4);
	bos.WriteInteger(qlp, 4);
	bis.ReadInteger(&qlpCoeffShift, 5);
	bos.WriteInteger(qlpCoeffShift, 5);

	unencQLPCoeffsBitSize = qlp * GetPredictorOrder(sfh);
	if (unencQLPCoeffsBitSize != 0) CopyBits(bis, bos, unencQLPCoeffsBitSize);

	CopyResidual(bis, bos, fh, msi, sfh);
}

void Trimmer::CopyResidual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	uint8_t codingMethod = 0;
	bis.ReadInteger(&codingMethod, 2);
	bos.WriteInteger(codingMethod, 2);
	switch (codingMethod)
	{
		case RICE:
			CopyRiceResidual(bis, bos, fh, msi, sfh);
		break;
		case RICE2:
			CopyRice2Residual(bis, bos, fh, msi, sfh);
		break;
		default:
			throw UnknownResudialCodingMethod();
		break;
	}

}

void Trimmer::CopyRiceResidual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	uint8_t partitionOrder = 0;
	bis.ReadInteger(&partitionOrder, 4);
	bos.WriteInteger(partitionOrder, 4);

	size_t partitionsCount = pow(2, partitionOrder);
	
	
	for (size_t partitionN = 0; partitionN < partitionsCount; partitionN++)
	{
		uint8_t riceParameter = 0;
		bis.ReadInteger(&riceParameter, 4);
		bos.WriteInteger(riceParameter, 4);
	
		bool isUnencoded = false;
		uint8_t unencBitsPerSample = 0;
		if (riceParameter == 0b1111) 
		{
			isUnencoded = true;
			bis.ReadInteger(&unencBitsPerSample, 5);
			bos.WriteInteger(unencBitsPerSample, 5);
		}

		size_t samplesCount = 0;
		if (partitionOrder == 0)
		{
			samplesCount = GetBlockSize(fh, msi) - GetPredictorOrder(sfh);
		}
		else if (partitionN != 0) // this is not first partition of subframe ?
		{
			samplesCount = GetBlockSize(fh, msi) / pow(2, partitionOrder);
		}
		else
		{
			samplesCount = GetBlockSize(fh, msi) / pow(2, partitionOrder) - GetPredictorOrder(sfh);
		}

		size_t residualBitSize = 0;

		if (isUnencoded)
		{
			residualBitSize = samplesCount * unencBitsPerSample;
		}
		else 
		{
			residualBitSize = samplesCount * GetBitsPerSample(fh, msi);
		}
		CopyBits(bis, bos, residualBitSize);

	}
}

void Trimmer::CopyRice2Residual(BitIStream& bis, BitOStream& bos, FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	cout << "Rice 2" << endl;
}

uint8_t Trimmer::GetPredictorOrder(FLACSubframeHeader * sfh)
{
	return sfh->Order;
}

uint32_t Trimmer::GetBlockSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{
	if (fh->BlockSize == 0b0000) return msi->MinBlockSize;
	else if (fh->BlockSize == 0b0001) return 192;
	else if (0b0010 <= fh->BlockSize && fh->BlockSize <= 0b0101 ) return 576 * (pow(2, fh->BlockSize - 2));
	else if (fh->BlockSize == 0b0110 || fh->BlockSize == 0b0111) return (fh->VariableBlockSize + 1);
	else return 256 * pow(2, fh->BlockSize - 8);

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

uint16_t Trimmer::GetUnencSubblockBitSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi)
{
	return GetBlockSize(fh, msi) * GetBitsPerSample(fh, msi);
}

uint16_t Trimmer::GetWarmUpSamplesBitSize(FLACFrameHeader * fh, FLACMetaStreamInfo * msi, FLACSubframeHeader * sfh)
{
	return GetBitsPerSample(fh, msi) * GetPredictorOrder(sfh);
}



