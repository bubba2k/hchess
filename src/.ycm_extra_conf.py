import os
import ycm_core

flags = [
  '-Wall',
  '-Wextra',
  '-Wno-long-long',
  '-Wno-variadic-macros',
  '-ferror-limit=10000',
  '-std=c99',
  '-I',
  '../include',
  '-I',
  'include'
  ]

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', ]

def FlagsForFile( filename, **kwargs ):
  return {
  'flags': flags,
  'do_cache': True
  }
