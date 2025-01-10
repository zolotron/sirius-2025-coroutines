import os
import platform
import sys
from typing import List


def __skip_file(path: str) -> bool:
    if path.startswith('cmake-build-debug/'):
        return True
    if path.startswith('.svn/'):
        return True
    if path.startswith('.git/'):
        return True
    return False


def __normalize_file_path(path: str) -> str:
    path = path.replace('\\', '/')
    if path.startswith('./'):
        return path[2:]
    return path


def __list_files(extension: str) -> List[str]:
    result: List[str] = []
    for path, subdirs, files in os.walk('.'):
        for name in files:
            file_path: str = os.path.join(path, name)
            file_path = __normalize_file_path(file_path)
            if __skip_file(file_path):
                continue
            if name.endswith(extension):
                result.append(file_path)
    return result


def __list_sources() -> List[str]:
    return __list_files('.c')


def __list_headers() -> List[str]:
    return __list_files('.h')


def __write_file(path: str, content: str) -> None:
    with open(path, 'wb') as path:
        path.write(content.encode(encoding='utf-8'))


def __get_platform() -> str:
    return platform.system().lower()


def __get_arch() -> str:
    return platform.machine().lower()


def __create_build_dir() -> None:
    if not os.path.exists('cmake-build-debug'):
        os.mkdir('cmake-build-debug')


def __autogen_project() -> None:
    sources: List[str] = __list_sources()
    headers: List[str] = __list_headers()
    content: List[str] = []
    platform: str = __get_platform()
    arch: str = __get_arch()

    sources.sort()
    headers.sort()

    content.append('# autogen: use \'build.py autogen project\' to regenerate this file')
    content.append('cmake_minimum_required(VERSION 3.24)')
    content.append('project(libcorocore C)')
    content.append('')
    content.append('set(CMAKE_C_STANDARD 11)')
    content.append('')
    content.append('add_library(libcorocore STATIC)')
    content.append('set_target_properties(libcorocore PROPERTIES LINKER_LANGUAGE C)')
    content.append('target_include_directories(libcorocore PRIVATE .)')

    if platform == 'windows':
        content.append('add_compile_definitions(PLATFORM_WINDOWS)')
    elif platform == 'linux':
        content.append('add_compile_definitions(PLATFORM_LINUX)')
    else:
        print(f'WARNING: unsupported platform \'{platform}\'')
        content.append('add_compile_definitions(PLATFORM_UNSUPPORTED)')

    if arch in ['amd64', 'x86_64']:
        content.append('add_compile_definitions(ARCH_X86_64)')
    else:
        print(f'WARNING: unsupported architecture \'{arch}\'')
        content.append('add_compile_definitions(ARCH_UNSUPPORTED)')
    content.append('')
    content.append('target_sources(')
    content.append('    libcorocore')
    content.append('    PRIVATE')
    content.append('    # Header files')
    for file in headers:
        content.append(f'    {file}')
    content.append('    # Source files')
    for file in sources:
        content.append(f'    {file}')
    content.append(')')
    content.append('')
    content.append('add_custom_command(')
    content.append('    TARGET libcorocore')
    content.append('    POST_BUILD')
    content.append('    COMMAND mv liblibcorocore.a libcorocore.a')
    content.append(')')

    __write_file('CMakeLists.txt', '\n'.join(content) + '\n')
    __create_build_dir()


def main(args: List[str]) -> None:
    argc: int = len(args)
    if argc == 3:
        if args[1] == 'autogen' and args[2] == 'project':
            __autogen_project()


if __name__ == '__main__':
    main(sys.argv)
