#ifdef __WAND__
target[type[application]name[transposer]platform[;GNU/Linux]]
#endif

#include <mustudio/client.h>
#include <mustudio/midi_input_exported.h>
#include <mustudio/midi_output_exported.h>
#include <mustudio/midi_event.h>
#include <algorithm>

#include <cstdio>
#include <unistd.h>

class Transposer:public MuStudio::Client
	{
	public:
		Transposer(int amount):MuStudio::Client("Transposer")
			,midi_in(*this,"MIDI in")
			,midi_out(*this,"MIDI out")
			,m_amount(amount)
			{
			activate();
			}

		int onProcess(size_t n_frames)
			{
			MuStudio::MIDI::Event event_in;
			bool event_has=midi_in.eventFirstGet(event_in,n_frames);

			midi_out.messageWritePrepare(n_frames);
			size_t now=0;
			while(n_frames)
				{
				if(event_has && event_in.time==now)
					{
					if((event_in.data.bytes[0]&0xf0)==0x80
						|| (event_in.data.bytes[0]&0xf0)==0x90)
						{
						int key=event_in.data.bytes[1];
						key+=m_amount;
						event_in.data.bytes[1]=std::max(0,std::min(key,127));
						}
					midi_out.messageWrite(event_in);
					event_has=midi_in.eventNextGet(event_in);
					}
				--n_frames;
				++now;
				}
			return 0;
			}

		~Transposer()
			{
			deactivate();
			}



	private:
		MuStudio::MIDI::InputExported midi_in;
		MuStudio::MIDI::OutputExported midi_out;
		int m_amount;
	};

void errlog(const char* message)
	{fprintf(stderr,"%s\n",message);}


int main(int argc,char* argv[])
	{
	if(argc<2)
		{return -1;}
	MuStudio::Client::setErrorHandler(errlog);

	Transposer output(atoi(argv[1]));

	while(1)
		{
		sleep(1);
		}

	return 0;
	}
