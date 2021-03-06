# Models music (an audio stream) that has been loaded from disk
# and kept in storage
class Music < Store
  extend Store::Forward

  # Loads music/audio stream.
  # Supports .ogg,  .it, .mod, .s3m and .xm files.
  def self.load(name, vpath, buffer_count=8, samples=8000)
    load_something(forward_name(name), vpath, self) do | nid |
      Eruta::Store.load_audio_stream(nid, vpath, buffer_count, samples)
    end 
  end

  # Returns true if music is playing, false if not
  def self.playing?
    return Eruta::Audio.music_playing?
  end

  # Plays this music as the current music starting from the beginning
  # without transition
  def play!
    Eruta::Audio.music_id = @id
    return Eruta::Audio.play_music
  end

  # Stops playing the music immediately 
  def self.stop!
    Eruta::Audio.stop_music
  end
  
  # Stops playing the music immediately 
  def stop!
    Music.stop!
  end



end
