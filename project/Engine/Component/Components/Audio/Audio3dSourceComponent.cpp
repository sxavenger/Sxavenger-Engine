#include "Audio3dSourceComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* entity
#include "../../Entity/MonoBehaviour.h"

//* audio
#include "AudioController.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Audio3dSourceComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Audio3dSourceComponent::Play() {

	auto clip = audio_.WaitRequire();
	IXAudio2SourceVoice* source = sAudioController->CreateSource(AudioHandle::Tag::SE, clip->GetFormat());

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = clip->GetBufferPointer();
	buf.AudioBytes = static_cast<UINT32>(clip->GetBufferSize());
	buf.Flags      = XAUDIO2_END_OF_STREAM;
	buf.LoopCount  = isLoop_ ? XAUDIO2_LOOP_INFINITE : 0;

	auto hr = source->SubmitSourceBuffer(&buf);
	DxObject::Assert(hr, L"IXAudio2SourceVoice submit buffer failed.");

	handle_ = std::make_unique<AudioHandle>(source);
	handle_->SetVolume(volume_);
}

void Audio3dSourceComponent::Update(const X3DAUDIO_LISTENER& listener) {
	if (handle_ == nullptr) { // 再生されていない場合
		return;
	}

	const X3DAUDIO_EMITTER emitter = GetEmitter();

	std::array<float, XAUDIO2_MAX_AUDIO_CHANNELS> matrix = {};

	X3DAUDIO_DSP_SETTINGS setting = {};
	setting.SrcChannelCount     = emitter.ChannelCount;
	setting.DstChannelCount     = sAudioController->GetVoiceDetails(AudioHandle::Tag::SE).InputChannels;
	setting.pMatrixCoefficients = matrix.data();

	sAudioController->CalculateAudio3d(
		listener,
		emitter,
		setting
	);

	auto hr = handle_->GetSource()->SetOutputMatrix(
		sAudioController->GetSubmix(AudioHandle::Tag::SE),
		setting.SrcChannelCount,
		setting.DstChannelCount,
		setting.pMatrixCoefficients
	);
	DxObject::Assert(hr, L"IXAudio2SourceVoice set output matrix failed.");

	handle_->Play();
}

void Audio3dSourceComponent::SetVolume(float volume) {
	volume_ = volume;

	if (handle_ != nullptr) {
		handle_->SetVolume(volume_);
	}
}

const TransformComponent* Audio3dSourceComponent::RequireTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}

const X3DAUDIO_EMITTER Audio3dSourceComponent::GetEmitter() const {

	Vector3f position = RequireTransform()->GetPosition();

	X3DAUDIO_EMITTER emitter = {};
	emitter.Position             = { position.x, position.y, position.z };
	emitter.OrientFront          = { kForward3<float>.x, kForward3<float>.y, kForward3<float>.z };
	emitter.OrientTop            = { kUp3<float>.x, kUp3<float>.y, kUp3<float>.z };
	emitter.Velocity             = { 0.0f, 0.0f, 0.0f };
	emitter.ChannelCount         = static_cast<UINT32>(channel_);
	emitter.pChannelAzimuths     = kDefaultStereoAzimuths.data();

	emitter.CurveDistanceScaler  = 0.1f;
	//emitter.ChannelRadius        = 0.2f;

	return emitter;
}
