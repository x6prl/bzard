#!/usr/bin/env python3
import os
import sys
import argparse

# Список замен: (старое_значение, новое_значение)
REPLACEMENTS = [
    ("bzard", "bzard"),
    ("bzard", "bzard"),
    ("BzardDBusService", "BzardDBusService"),
    ("BzardExpirationController", "BzardExpirationController"),
    ("BzardHistory", "BzardHistory"),
    ("BzardNotificationModifiers", "BzardNotificationModifiers"),
    ("BzardNotifications", "BzardNotifications"),
    ("BzardThemes", "BzardThemes"),
    ("BzardTopDown", "BzardTopDown"),
    ("BzardTrayIcon", "BzardTrayIcon"),
]

# Расширения файлов, которые будем обрабатывать как текстовые
ALLOWED_EXTENSIONS = {
    ".cpp", ".c", ".h", ".hpp", ".qml", ".qrc", ".desktop", ".spec",
    ".md", ".txt", ".json", ".ini", ".pro", ".cmake", ".py"
}

def is_text_file(file_path):
    """
    Проверяет, можно ли прочитать файл как UTF-8.
    Если не получается – предполагает, что файл бинарный.
    """
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            f.read()
        return True
    except Exception:
        return False

def process_file(file_path, dry_run=False):
    """
    Читает файл, производит замену строк согласно REPLACEMENTS и
    перезаписывает файл, если содержимое изменилось.
    """
    ext = os.path.splitext(file_path)[1].lower()
    if ext not in ALLOWED_EXTENSIONS:
        return

    if not is_text_file(file_path):
        return

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()
    except Exception as e:
        print(f"Ошибка чтения файла {file_path}: {e}")
        return

    new_content = content
    for old, new in REPLACEMENTS:
        new_content = new_content.replace(old, new)

    if new_content != content:
        if dry_run:
            print(f"[DRY RUN] Файл {file_path} был бы обновлён")
        else:
            try:
                with open(file_path, "w", encoding="utf-8") as f:
                    f.write(new_content)
                print(f"Обновлён файл: {file_path}")
            except Exception as e:
                print(f"Ошибка записи файла {file_path}: {e}")

def rename_file_if_needed(file_path, dry_run=False):
    """
    Переименовывает файл, если его имя начинается с 'iq' или 'IQ',
    заменяя префикс на 'bzard' или 'Bzard' соответственно.
    Возвращает новый путь, если файл переименован, иначе исходный путь.
    """
    directory, filename = os.path.split(file_path)
    new_filename = None

    if filename.startswith("iq"):
        new_filename = "bzard" + filename[2:]
    elif filename.startswith("IQ"):
        new_filename = "Bzard" + filename[2:]

    if new_filename and new_filename != filename:
        new_file_path = os.path.join(directory, new_filename)
        if dry_run:
            print(f"[DRY RUN] Файл {file_path} был бы переименован в {new_file_path}")
            return file_path
        try:
            os.rename(file_path, new_file_path)
            print(f"Переименован файл: {file_path} -> {new_file_path}")
            return new_file_path
        except Exception as e:
            print(f"Ошибка переименования файла {file_path}: {e}")
            return file_path
    return file_path

def process_directory(root_dir, dry_run=False, rename_only=False):
    """
    Рекурсивно проходит по всем файлам в указанной директории,
    производит замену текста (если не установлен флаг rename_only)
    и переименование файлов.
    """
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            file_path = os.path.join(dirpath, filename)
            if not rename_only:
                process_file(file_path, dry_run=dry_run)
            # Переименование выполняем всегда, независимо от rename_only
            rename_file_if_needed(file_path, dry_run=dry_run)

def main():
    parser = argparse.ArgumentParser(
        description="Скрипт для автоматического рефакторинга проекта: замена префиксов и переименование файлов."
    )
    parser.add_argument("directory", help="Путь к корневой директории проекта")
    parser.add_argument("--dry-run", action="store_true", help="Режим проверки: вывод изменений без записи")
    parser.add_argument("--rename-only", action="store_true", help="Выполнять только переименование файлов без изменения содержимого")
    args = parser.parse_args()

    if not os.path.isdir(args.directory):
        print(f"Указанный путь не является директорией: {args.directory}")
        sys.exit(1)

    process_directory(args.directory, dry_run=args.dry_run, rename_only=args.rename_only)

if __name__ == "__main__":
    main()
